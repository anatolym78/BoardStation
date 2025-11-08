#ifndef SESSIONFRAME_H
#define SESSIONFRAME_H

#include <QFrame>

#include "./../ViewModel/Session.h"

class ParametersTreeView;
class ChartsPanel;
class PlayerView;

class SessionFrame : public QFrame
{
    Q_OBJECT
public:
    explicit SessionFrame(QWidget *parent = nullptr);

    void attachModels(Session* session);
private:
    ParametersTreeView* m_parametersTree;
    ChartsPanel* m_chartsPanel;
    PlayerView* m_playerView;
};

#endif // SESSIONFRAME_H
