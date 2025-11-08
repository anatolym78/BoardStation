#include "SessionFrame.h"
#include "ParametersTreeView.h"
#include "ChartsPanel.h"
#include "PlayerView.h"
#include "BoardParametersTreeModelProxy.h"

#include <QGridLayout>

SessionFrame::SessionFrame(QWidget *parent)
    : QFrame(parent)
{
    m_parametersTree = new ParametersTreeView(this);
    m_chartsPanel = new ChartsPanel(this);
    m_playerView = new PlayerView(this);

    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(m_parametersTree, 0, 0);
    mainLayout->addWidget(m_chartsPanel, 0, 1);
    mainLayout->addWidget(m_playerView, 1, 0, 1, 2);

    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 3);
}

void SessionFrame::attachModels(Session* session)
{
    auto originalModel = session->parametersModel();
    auto proxyModel = new BoardParametersTreeModelProxy(this);
    proxyModel->setSourceModel(originalModel);
    m_parametersTree->setModel(proxyModel);
}
