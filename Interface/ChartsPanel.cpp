#include "ChartsPanel.h"
#include "ParametersChartView.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QWidget>
#include <QVBoxLayout>
#include <QLayoutItem>

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>

#include <QPushButton>

#include "./../Model/Parameters/Tree/ParameterTreeArrayItem.h"

ChartsPanel::ChartsPanel(QWidget *parent)
	: QFrame(parent)
{
	m_scrollArea = new QScrollArea(this);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setFrameShape(QFrame::NoFrame);

	m_scrollContent = new QWidget();
	m_gridLayout = new QGridLayout(m_scrollContent);
	m_gridLayout->setSpacing(5);
	m_gridLayout->setContentsMargins(10, 10, 10, 10);

	m_scrollArea->setWidget(m_scrollContent);

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(m_scrollArea);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	// Следим за изменением ширины области просмотра, чтобы пересчитывать размеры ячеек (квадратов)
	m_scrollArea->viewport()->installEventFilter(this);
}

void ChartsPanel::setModel(ChatViewGridModel* chartsModel)
{
	m_chartsModel = chartsModel;

	connect(m_chartsModel, &ChatViewGridModel::parameterAdded, this, &ChartsPanel::onParameterAdded);
	connect(m_chartsModel, &ChatViewGridModel::parameterNeedToRemove, this, &ChartsPanel::onParameterRemoved);
}

void ChartsPanel::onParameterAdded(int chartIndex, ParameterTreeItem* parameter, const QColor& color)
{
	auto parameterFullName = parameter->fullName();

	if (m_chartsModel->hasSeries(parameterFullName)) return;

	auto row = chartIndex / m_columnCount;
	auto column = chartIndex % m_columnCount;

	// create chart
	auto chartView = new ParametersChartView(chartIndex, row, column, this);
	chartView->setRenderHint(QPainter::Antialiasing, true);
	auto chart = new QtCharts::QChart();
	chart->setBackgroundBrush(QBrush());
	chartView->setChart(chart);
	m_gridLayout->addWidget(chartView, row, column);

	// create timeaxis
	auto timeAxis = new QtCharts::QDateTimeAxis(chart);
	timeAxis->setFormat("hh:mm:ss");
	timeAxis->setTickCount(3);
	chart->addAxis(timeAxis, Qt::AlignBottom);

	//create value axis
	auto valueAxis = new QtCharts::QValueAxis(chart);
	valueAxis->setTickCount(3);
	valueAxis->setMin(10);
	valueAxis->setMax(30);
	chart->addAxis(valueAxis, Qt::AlignLeft);


	// create series
	if (parameter->type() == ParameterTreeItem::ItemType::History)
	{
		auto series = new QtCharts::QLineSeries();
		chart->addSeries(series);
		series->attachAxis(timeAxis);
		series->attachAxis(valueAxis);

		// add series to model
		m_chartsModel->addSeriesToChart(chartIndex, parameterFullName,
			Qt::red, series, timeAxis, valueAxis);
	}
	// create series list
	if (parameter->type() == ParameterTreeItem::ItemType::Array)
	{
		chart->setTitle(parameterFullName);
		auto parameterList = static_cast<ParameterTreeArrayItem*>(parameter);

		for (auto childParameter : parameterList->children())
		{
			if(childParameter->type() != ParameterTreeItem::ItemType::History) continue;
		
			auto parameterHistory = static_cast<ParameterTreeHistoryItem*>(childParameter);

			auto parameterName = parameterHistory->fullName();
			auto series = new QtCharts::QLineSeries();
			chart->addSeries(series);
			series->attachAxis(timeAxis);
			series->attachAxis(valueAxis);

			// add series to model
			m_chartsModel->addSeriesToChart(chartIndex, parameterName,
				Qt::blue, series, timeAxis, valueAxis);
		}
	}

	// После добавления - пересчитать размеры ячеек, чтобы они вписались по ширине и имели квадратную форму
	updateCellSizes();
}

void ChartsPanel::onParameterRemoved(int chartIndex, const QString& label)
{
	for (auto i = 0; i < m_gridLayout->count(); i++)
	{
		auto item = m_gridLayout->itemAt(i);
		auto chartView = (ParametersChartView*)(item->widget());
		if (chartView->chartIndex() == i)
		{
			auto chart = chartView->chart();
			for (auto seires : chart->series())
			{
				if (seires->name() == label)
				{
					chart->removeSeries(seires);

					break;
				}
			}

			if (chart->series().isEmpty())
			{
				m_gridLayout->removeWidget(chartView);

				chartView->deleteLater();
			}
		}
	}

	return;
	// Вычисляем позицию в grid layout из индекса чарта
	auto row = chartIndex / m_columnCount;
	auto column = chartIndex % m_columnCount;
	
	// Получаем item из layout по позиции
	QLayoutItem* item = m_gridLayout->itemAtPosition(row, column);
	
	if (item != nullptr)
	{
		// Получаем виджет из item
		QWidget* widget = item->widget();
		
		if (widget != nullptr)
		{
			auto chartView = (ParametersChartView*)widget;
			// Удаляем виджет из layout
			// removeWidget() автоматически удаляет соответствующий item из layout
			m_gridLayout->removeWidget(widget);
			
			// Удаляем сам виджет (освобождаем память)
			widget->deleteLater();
		}
	}
	
	// После удаления - пересчитать размеры оставшихся ячеек
	updateCellSizes();
}

bool ChartsPanel::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == m_scrollArea->viewport() && event->type() == QEvent::Resize)
	{
		//updateCellSizes();
	}
	return QFrame::eventFilter(watched, event);
}

void ChartsPanel::updateCellSizes()
{
	if (!m_gridLayout || !m_scrollArea) return;

	const int viewportWidth = m_scrollArea->viewport()->width();
	const QMargins margins = m_gridLayout->contentsMargins();
	const int hSpacing = m_gridLayout->horizontalSpacing();

	// Ширина контента с учётом внутренних отступов
	const int contentWidth = viewportWidth - margins.left() - margins.right();
	if (contentWidth <= 0 || m_columnCount <= 0) return;

	// Ширина одной ячейки с учётом горизонтальных промежутков
	const int totalSpacing = hSpacing * (m_columnCount - 1);
	const int cellWidth = (contentWidth - totalSpacing) / m_columnCount;
	if (cellWidth <= 0) return;

	// Пробегаем все элементы сетки и задаём им квадратный фиксированный размер
	const int itemCount = m_gridLayout->count();
	for (int i = 0; i < itemCount; ++i)
	{
		if (auto item = m_gridLayout->itemAt(i))
		{
			if (auto w = item->widget())
			{
				w->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
				w->setFixedSize(cellWidth, cellWidth);
			}
		}
	}

	// Обновление размеров контента для корректной вертикальной прокрутки
	m_scrollContent->adjustSize();
}