#ifndef SESSIONFRAME_H
#define SESSIONFRAME_H

#include <QFrame>

#include "./../ViewModel/Session.h"

class ParametersTreeView;
class ChartsPanel;
class PlayerView;
class QPushButton;
class QVBoxLayout;

class SessionFrame : public QFrame
{
    Q_OBJECT
public:
    explicit SessionFrame(Session* session, QWidget *parent = nullptr);

    void attachModels(Session* session);

private slots:
    void onAddChartButtonClicked();

private:
    Session* m_session;
    ParametersTreeView* m_parametersTree;
    ChartsPanel* m_chartsPanel;
    PlayerView* m_playerView;
    QPushButton* m_addChartButton;
    QVBoxLayout* m_parametersTreeLayout;
};

#endif // SESSIONFRAME_H
