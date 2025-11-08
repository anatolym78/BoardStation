#include "PlayerView.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QSlider>
#include <QLabel>

PlayerView::PlayerView(QWidget *parent)
	: QWidget(parent)
{
	m_playPauseButton = new QToolButton(this);
	m_playPauseButton->setIcon(QIcon(":/Resources/run_32.png"));
	m_playPauseButton->setIconSize(QSize(32, 32));
	m_playPauseButton->setToolTip("Play/Pause");
	m_playPauseButton->setCheckable(true);

	m_stopButton = new QToolButton(this);
	m_stopButton->setIcon(QIcon(":/Resources/stop_32.png"));
	m_stopButton->setIconSize(QSize(32, 32));
	m_stopButton->setToolTip("Stop");

	m_positionSlider = new QSlider(Qt::Horizontal, this);
	m_positionSlider->setMinimum(0);
	m_positionSlider->setMaximum(100);

	m_infoLabel = new QLabel(this);
	auto font = QFont();
	font.setPointSize(11);
	m_infoLabel->setFont(font);
	m_infoLabel->setText("00:00 / 00:00");

	QHBoxLayout* mainLayout = new QHBoxLayout(this);
	mainLayout->addWidget(m_playPauseButton);
	mainLayout->addWidget(m_stopButton);
	mainLayout->addWidget(m_positionSlider);
	mainLayout->addWidget(m_infoLabel);
	mainLayout->setContentsMargins(5, 5, 5, 5);
	mainLayout->setSpacing(5);

	connect(m_playPauseButton, &QToolButton::toggled, this, &PlayerView::onPlayButtonToggled);
}

void PlayerView::onPlayButtonToggled()
{
	if (m_playPauseButton->isChecked())
	{
		m_playPauseButton->setIcon(QIcon(":/Resources/pause_32.png"));

	}
	else
	{
		m_playPauseButton->setIcon(QIcon(":/Resources/run_32.png"));

	}
}
