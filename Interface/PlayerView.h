#ifndef PLAYERVIEW_H
#define PLAYERVIEW_H

#include <QWidget>

class QPushButton;
class QToolButton;
class QSlider;
class QLabel;
class DataPlayer;

class PlayerView : public QWidget
{
	Q_OBJECT
public:
	explicit PlayerView(QWidget *parent = nullptr);

	void onPlayButtonToggled();

	// Подключение к модели плеера
	void setPlayer(DataPlayer* player);

private:
	void updateInfoLabel(double elapsedSeconds, double durationSeconds);

private:
	QPushButton* m_playPauseButton;
	QPushButton* m_stopButton;
	QSlider* m_positionSlider;
	QLabel* m_infoLabel;
	DataPlayer* m_player = nullptr;
};

#endif // PLAYERVIEW_H
