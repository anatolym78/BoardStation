#include "ChartsPanel.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QWidget>
#include <QVBoxLayout>

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>

#include <QPushButton>

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
}

void ChartsPanel::setModel(ChatViewGridModel* chartsModel)
{
	m_chartsModel = chartsModel;

	connect(m_chartsModel, &ChatViewGridModel::chartAdded, this, &ChartsPanel::onChartAdded);
}

void ChartsPanel::onChartAdded(const QString& parameterFullName)
{
	auto chartView = new QtCharts::QChartView(this);
	chartView->setRenderHint(QPainter::Antialiasing, true);
	auto chart = new QtCharts::QChart();
	//chart->setTitle(chartName);
	chartView->setChart(chart);
	auto series = new QtCharts::QLineSeries();
	chart->addSeries(series);
	m_gridLayout->addWidget(chartView, 0, 0);

	auto timeAxis = new QtCharts::QDateTimeAxis(series);
	timeAxis->setFormat("hh:mm:ss");
	timeAxis->setTickCount(3);
	series->attachAxis(timeAxis);
	auto valueAxis = new QtCharts::QValueAxis(series);
	valueAxis->setTickCount(3);
	series->attachAxis(valueAxis);

	m_chartsModel->addSeriesToChart(0, parameterFullName, Qt::red, series, timeAxis, valueAxis);
}
