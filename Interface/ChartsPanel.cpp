#include "ChartsPanel.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLayoutItem>
#include <QDebug>
#include <algorithm>

#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>

#include <QPushButton>

#include "./../Model/Parameters/Tree/ParameterTreeArrayItem.h"

ChartsPanel::ChartsPanel(QWidget *parent)
	: QFrame(parent)
{
	// Верхняя панель с элементами управления
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	QHBoxLayout* topBarLayout = new QHBoxLayout();
	topBarLayout->setContentsMargins(2, 2, 2, 0);

	m_oneColumnButton = new QPushButton("One column", this);
	m_twoColumnButton = new QPushButton("Two column", this);
	connect(m_oneColumnButton, &QPushButton::clicked, this, &ChartsPanel::onOneColumnClicked);
	connect(m_twoColumnButton, &QPushButton::clicked, this, &ChartsPanel::onTwoColumnClicked);
	
	// Устанавливаем начальное состояние: две колонки активны
	m_twoColumnButton->setEnabled(false);

	QPushButton* mergeButton = new QPushButton("Merge charts", this);
	connect(mergeButton, &QPushButton::clicked, this, &ChartsPanel::onMergeChartsClicked);

	topBarLayout->addWidget(m_oneColumnButton, 0, Qt::AlignLeft);
	topBarLayout->addSpacing(2);
	topBarLayout->addWidget(m_twoColumnButton, 0, Qt::AlignLeft);
	topBarLayout->addSpacing(2);
	topBarLayout->addWidget(mergeButton, 0, Qt::AlignLeft);
	topBarLayout->addStretch(1);

	mainLayout->addLayout(topBarLayout);

	m_scrollArea = new QScrollArea(this);
	m_scrollArea->setWidgetResizable(true);
	m_scrollArea->setFrameShape(QFrame::NoFrame);

	m_scrollContent = new QWidget();
	m_gridLayout = new QGridLayout(m_scrollContent);
	m_gridLayout->setSpacing(2);
	m_gridLayout->setContentsMargins(2, 2, 2, 2);

	m_scrollArea->setWidget(m_scrollContent);

	mainLayout->addWidget(m_scrollArea);

	// Следим за изменением ширины области просмотра, чтобы пересчитывать размеры ячеек (квадратов)
	m_scrollArea->viewport()->installEventFilter(this);
}

void ChartsPanel::setModel(ChatViewGridModel* chartsModel)
{
	m_chartsModel = chartsModel;

	connect(m_chartsModel, &ChatViewGridModel::parameterAdded, this, &ChartsPanel::onParameterAdded);
	connect(m_chartsModel, &ChatViewGridModel::parameterNeedToRemove, this, &ChartsPanel::onParameterRemoved);
	connect(m_chartsModel, &ChatViewGridModel::parametersNeedToMove, this, &ChartsPanel::onParameterMoved);
}

void ChartsPanel::onParameterAdded(int chartIndex, ParameterTreeItem* parameter, const QColor& color)
{
	auto parameterFullName = parameter->fullName();

	if (m_chartsModel->hasSeries(parameterFullName)) return;

	auto row = chartIndex / m_columnCount;
	auto column = chartIndex % m_columnCount;

	// create chart
	auto chartView = new ParametersChartView(chartIndex, row, column, this);
	chartView->setModel(m_chartsModel);
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
	updateCellSizes();
}

void ChartsPanel::onParameterMoved(int targetChartIndex, const QStringList& labels)
{
	auto targetChartView = getChartView(targetChartIndex);
	if (!targetChartView) return;

	auto targetChart = targetChartView->chart();
	if (!targetChart) return;

	const QSet<QString> labelsSet = QSet<QString>(labels.begin(), labels.end());

	// Обходим все графики, кроме целевого, и переносим подходящие серии
	for (int i = 0; i < m_gridLayout->count(); ++i)
	{
		if (auto item = m_gridLayout->itemAt(i))
		{
			auto sourceView = qobject_cast<ParametersChartView*>(item->widget());
			if (!sourceView || sourceView == targetChartView) continue;

			auto sourceChart = sourceView->chart();
			if (!sourceChart) continue;

			// Копируем список, чтобы безопасно модифицировать исходный график
			const auto sourceSeriesList = sourceChart->series();
			bool anyMovedFromSource = false;
			for (auto series : sourceSeriesList)
			{
				if (!series) continue;
				if (!labelsSet.contains(series->name())) continue;

				// Сначала удаляем из исходного графика
				sourceChart->removeSeries(series);
				// Затем добавляем в целевой график
				targetChart->addSeries(series);
				// Подключаем оси целевого графика
				if (targetChart->axisX()) series->attachAxis(targetChart->axisX());
				if (targetChart->axisY()) series->attachAxis(targetChart->axisY());

				anyMovedFromSource = true;
			}

			// Если исходный график опустел — удаляем виджет
			if (anyMovedFromSource && sourceChart->series().isEmpty())
			{
				m_gridLayout->removeWidget(sourceView);
				sourceView->deleteLater();
			}
		}
	}

	targetChartView->setSelected(false);
	updateCellSizes();
}

ParametersChartView* ChartsPanel::getChartView(int chartIndex)
{
	for (int i = 0; i < m_gridLayout->count(); ++i)
	{
		if (auto item = m_gridLayout->itemAt(i))
		{
			auto view = qobject_cast<ParametersChartView*>(item->widget());
			if (view && view->chartIndex() == chartIndex)
			{
				return view;
			}
		}
	}
	return nullptr;
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

void ChartsPanel::onOneColumnClicked()
{
	m_columnCount = 1;
	m_oneColumnButton->setEnabled(false);
	m_twoColumnButton->setEnabled(true);
	relayoutChartsGrid();
}

void ChartsPanel::onTwoColumnClicked()
{
	m_columnCount = 2;
	m_oneColumnButton->setEnabled(true);
	m_twoColumnButton->setEnabled(false);
	relayoutChartsGrid();
}

void ChartsPanel::onMergeChartsClicked()
{
	m_chartsModel->mergeCharts();
}

void ChartsPanel::relayoutChartsGrid()
{
	if (!m_gridLayout) return;

	// Собираем все текущие виды графиков
	QList<ParametersChartView*> views;
	for (int i = 0; i < m_gridLayout->count(); ++i)
	{
		if (auto item = m_gridLayout->itemAt(i))
		{
			if (auto v = qobject_cast<ParametersChartView*>(item->widget()))
			{
				views.append(v);
			}
		}
	}

	// Стабильный порядок по chartIndex
	std::sort(views.begin(), views.end(), [](ParametersChartView* a, ParametersChartView* b)
	{
		return a->chartIndex() < b->chartIndex();
	});

	// Убираем из layout, затем добавим заново согласно новым колонкам
	for (auto v : views)
	{
		m_gridLayout->removeWidget(v);
	}

	for (int idx = 0; idx < views.size(); ++idx)
	{
		int row = m_columnCount > 0 ? idx / m_columnCount : 0;
		int col = m_columnCount > 0 ? idx % m_columnCount : 0;
		m_gridLayout->addWidget(views.at(idx), row, col);
	}

	updateCellSizes();
}