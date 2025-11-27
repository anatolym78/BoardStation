#include "SessionFrame.h"
#include "ParametersTreeView.h"
#include "ChartsPanel.h"
#include "PlayerView.h"

#include <QGridLayout>
#include <QItemSelectionModel>
#include <QVBoxLayout>
#include <QPushButton>
#include "Model/Parameters/Tree/ParameterTreeHistoryItem.h"

SessionFrame::SessionFrame(Session* session, QWidget *parent) : QFrame(parent)
{
	m_session = session;

	// Создаем вертикальный layout для дерева параметров
	m_parametersTreeLayout = new QVBoxLayout();
	m_parametersTreeLayout->setContentsMargins(0, 0, 0, 0);
	m_parametersTreeLayout->setSpacing(5);

	// Создаем кнопку "Добавить график"
	m_addChartButton = new QPushButton(tr("Toggle chart"), this);
	m_addChartButton->setIcon(QIcon(":/Resources/toggle_parameter_16.png"));
	connect(m_addChartButton, &QPushButton::clicked, this, &SessionFrame::onAddChartButtonClicked);

	// Создаем дерево параметров
	m_parametersTree = new ParametersTreeView(this);

	// Добавляем кнопку и дерево в вертикальный layout
	m_parametersTreeLayout->addWidget(m_addChartButton);
	m_parametersTreeLayout->addWidget(m_parametersTree, 1); // 1 - растягиваем дерево

	// Создаем контейнер для вертикального layout
	QWidget* parametersContainer = new QWidget(this);
	parametersContainer->setLayout(m_parametersTreeLayout);

	m_chartsPanel = new ChartsPanel(this);
	m_playerView = new PlayerView(this);

	QGridLayout* mainLayout = new QGridLayout(this);
	mainLayout->addWidget(parametersContainer, 0, 0);
	mainLayout->addWidget(m_chartsPanel, 0, 1);
	mainLayout->addWidget(m_playerView, 1, 0, 1, 2);

	mainLayout->setColumnStretch(0, 3);
	mainLayout->setColumnStretch(1, 6);

	attachModels(m_session);

	connect(m_parametersTree, &ParametersTreeView::itemHovered, m_chartsPanel, &ChartsPanel::onParameterItemHovered);

}

void SessionFrame::attachModels(Session* session)
{
	auto model = session->parametersModel();
	m_parametersTree->setModel(model);
	
	// Устанавливаем selectionModel из Session в представление
	auto selectionModel = session->parametersSelectionModel();
	if (selectionModel)
	{
		m_parametersTree->setSelectionModel(selectionModel);
	}

	m_chartsPanel->setModel(session->chartsModel());

	// Подключаем плеер к PlayerView
	if (session->player())
	{
		m_playerView->setPlayer(session->player());
	}
}

void SessionFrame::onAddChartButtonClicked()
{
	if (m_session)
	{
		m_session->createChartFromSelectedParameter();
	}
}
