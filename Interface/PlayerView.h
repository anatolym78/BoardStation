#ifndef PLAYERVIEW_H
#define PLAYERVIEW_H

#include <QWidget>

class QToolButton;
class QSlider;
class QLabel;

class PlayerView : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerView(QWidget *parent = nullptr);

    void onPlayButtonToggled();
private:
    QToolButton* m_playPauseButton;
    QToolButton* m_stopButton;
    QSlider* m_positionSlider;
    QLabel* m_infoLabel;
};

#endif // PLAYERVIEW_H
