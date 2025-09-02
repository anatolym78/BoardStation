#include "ChartBuilder.h"
#include <QDebug>
#include <QPen>
#include <QTimer>

ChartBuilder::ChartBuilder(QObject *parent)
    : QObject(parent)
{
}

QChartView* ChartBuilder::createChart(const BoardParameter &parameter)
{
    if (!canCreateChart(parameter))
    {
        qWarning() << "ChartBuilder: Не удается создать график для параметра" << parameter.label;
        return nullptr;
    }
    
    // Создаем новый график
    QChart *chart = new QChart();
    buildChart(chart, parameter);
    
    // Создаем QChartView
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    qDebug() << "ChartBuilder: Создан график для параметра" << parameter.label;
    return chartView;
}

void ChartBuilder::updateChart(QChartView *chartView, const BoardParameter &parameter)
{
    if (!chartView || !canCreateChart(parameter))
    {
        qWarning() << "ChartBuilder: Не удается обновить график для параметра" << parameter.label;
        return;
    }
    
    QChart *chart = chartView->chart();
    if (!chart)
    {
        qWarning() << "ChartBuilder: QChartView не содержит QChart";
        return;
    }
    
    // Дополнительная проверка: график все еще валиден
    if (chart->series().isEmpty() && !parameter.values.isEmpty())
    {
        // Если серии нет, создаем график с нуля
        buildChart(chart, parameter);
        qDebug() << "ChartBuilder: Создан график для параметра" << parameter.label;
    }
    else if (!chart->series().isEmpty())
    {
        // Если серия есть, добавляем новые точки
        appendNewPoints(chartView, parameter);
        qDebug() << "ChartBuilder: Добавлены новые точки для параметра" << parameter.label;
    }
}

bool ChartBuilder::canCreateChart(const BoardParameter &parameter)
{
    if (!parameter.hasValues())
    {
        return false;
    }
    
    // Проверяем, что все значения имеют числовой тип
    for (const BoardParameterValue &value : parameter.values)
    {
        if (!value.value.canConvert<double>())
        {
            return false;
        }
    }
    
    return true;
}

void ChartBuilder::buildChart(QChart *chart, const BoardParameter &parameter)
{
    // Очищаем график от старых данных и осей
    chart->removeAllSeries();
    
    // Получаем все оси и удаляем их
    QList<QAbstractAxis*> axes = chart->axes();
    for (QAbstractAxis *axis : axes)
    {
        chart->removeAxis(axis);
    }
    
    // Создаем серию данных
    QLineSeries *series = createSeries(parameter);
    if (!series)
    {
        qWarning() << "ChartBuilder: Не удалось создать серию данных";
        return;
    }
    
    // Добавляем серию на график
    chart->addSeries(series);
    
    // Настраиваем оси
    setupAxes(chart, series, parameter);
    
    // Настраиваем внешний вид
    setupChartAppearance(chart, parameter);
}

void ChartBuilder::appendNewPoints(QChartView *chartView, const BoardParameter &parameter)
{
    QChart *chart = chartView->chart();
    if (!chart || chart->series().isEmpty())
    {
        return;
    }
    
    // Получаем существующую серию (предполагаем, что она одна)
    QLineSeries *series = qobject_cast<QLineSeries*>(chart->series().first());
    if (!series)
    {
        return;
    }
    
    // Дополнительная проверка: серия все еще валидна
    if (!series->parent())
    {
        qWarning() << "ChartBuilder: Серия была удалена";
        return;
    }
    
    // Определяем количество уже существующих точек
    int existingPoints = series->count();
    
    // Добавляем только новые точки
    for (int i = existingPoints; i < parameter.values.size(); ++i)
    {
        const BoardParameterValue &value = parameter.values[i];
        double timeValue = value.timestamp.toMSecsSinceEpoch() / 1000.0;
        double dataValue = value.value.toDouble();
        series->append(timeValue, dataValue);
    }
    
    // Обновляем диапазон осей для отображения новых данных
    QList<QAbstractAxis*> axes = chart->axes();
    QValueAxis *axisX = nullptr;
    QValueAxis *axisY = nullptr;
    
    // Ищем оси X и Y
    for (QAbstractAxis *axis : axes)
    {
        if (axis->alignment() == Qt::AlignBottom || axis->alignment() == Qt::AlignTop)
        {
            axisX = qobject_cast<QValueAxis*>(axis);
        }
        else if (axis->alignment() == Qt::AlignLeft || axis->alignment() == Qt::AlignRight)
        {
            axisY = qobject_cast<QValueAxis*>(axis);
        }
    }
    
    if (axisX && axisY && !parameter.values.isEmpty())
    {
        // Находим минимальные и максимальные значения времени
        double minTime = parameter.values.first().timestamp.toMSecsSinceEpoch() / 1000.0;
        double maxTime = parameter.values.last().timestamp.toMSecsSinceEpoch() / 1000.0;

        // Находим минимальные и максимальные значения данных
        double minValue = parameter.values.first().value.toDouble();
        double maxValue = minValue;

        for (const BoardParameterValue &value : parameter.values)
        {
            double val = value.value.toDouble();
            if (val < minValue) minValue = val;
            if (val > maxValue) maxValue = val;
        }

        // Добавляем небольшой отступ для лучшего отображения
        double timeRange = maxTime - minTime;
        double valueRange = maxValue - minValue;

        // Автоматически прокручиваем к новым данным
        axisX->setRange(minTime - timeRange * 0.05, maxTime + timeRange * 0.05);
        axisY->setRange(minValue - valueRange * 0.1, maxValue + valueRange * 0.1);

        // Если график уже отображается, можно добавить плавную анимацию прокрутки
        if (chart->animationOptions() & QChart::SeriesAnimations)
        {
            // Небольшая задержка для плавности
            QTimer::singleShot(100, [axisX, axisY, minTime, maxTime, minValue, maxValue, timeRange, valueRange]()
            {
                axisX->setRange(minTime - timeRange * 0.05, maxTime + timeRange * 0.05);
                axisY->setRange(minValue - valueRange * 0.1, maxValue + valueRange * 0.1);
            });
        }
    }
}

QLineSeries* ChartBuilder::createSeries(const BoardParameter &parameter)
{
    QLineSeries *series = new QLineSeries();
    series->setName(parameter.label);
    
    // Добавляем точки на график
    for (const BoardParameterValue &value : parameter.values)
    {
        double timeValue = value.timestamp.toMSecsSinceEpoch() / 1000.0; // Время в секундах
        double dataValue = value.value.toDouble();
        series->append(timeValue, dataValue);
    }
    
    // Устанавливаем красный цвет для линии
    QPen pen(Qt::red);
    pen.setWidth(2);
    series->setPen(pen);
    
    return series;
}

void ChartBuilder::setupAxes(QChart *chart, QLineSeries *series, const BoardParameter &parameter)
{
    // Создаем оси
    QValueAxis *axisX = new QValueAxis();
    QValueAxis *axisY = new QValueAxis();
    
    axisX->setTitleText(tr("Time (seconds)"));
    axisY->setTitleText(tr("%1 (%2)").arg(parameter.label, parameter.unit));
    
    // Привязываем оси к серии
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    
    // Добавляем оси на график
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    
    // Устанавливаем начальный диапазон осей, если есть данные
    if (!parameter.values.isEmpty())
    {
        double minTime = parameter.values.first().timestamp.toMSecsSinceEpoch() / 1000.0;
        double maxTime = parameter.values.last().timestamp.toMSecsSinceEpoch() / 1000.0;
        
        double minValue = parameter.values.first().value.toDouble();
        double maxValue = minValue;
        
        for (const BoardParameterValue &value : parameter.values)
        {
            double val = value.value.toDouble();
            if (val < minValue) minValue = val;
            if (val > maxValue) maxValue = val;
        }
        
        // Добавляем небольшой отступ для лучшего отображения
        double timeRange = maxTime - minTime;
        double valueRange = maxValue - minValue;
        
        if (timeRange > 0)
        {
            axisX->setRange(minTime - timeRange * 0.05, maxTime + timeRange * 0.05);
        }
        else
        {
            axisX->setRange(minTime - 1, maxTime + 1);
        }
        
        if (valueRange > 0)
        {
            axisY->setRange(minValue - valueRange * 0.1, maxValue + valueRange * 0.1);
        }
        else
        {
            axisY->setRange(minValue - 1, maxValue + 1);
        }
    }
}

void ChartBuilder::setupChartAppearance(QChart *chart, const BoardParameter &parameter)
{
    // Устанавливаем заголовок графика
    chart->setTitle(tr("Chart for %1").arg(parameter.label));
    
    // Включаем анимацию
    chart->setAnimationOptions(QChart::SeriesAnimations);
    
    // Настраиваем тему (можно сделать настраиваемой)
    chart->setTheme(QChart::ChartThemeLight);
    
    // Включаем легенду
    chart->legend()->show();
}
