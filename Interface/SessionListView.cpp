#include "SessionListView.h"

#include <QPainter>
#include <QFont>
#include <QFontMetrics>

SessionListView::SessionListView(QWidget *parent)
    : QListView(parent)
    , m_headerHeight(30)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAlternatingRowColors(true);
    
    // Устанавливаем отступы для viewport, чтобы заголовок не перекрывал содержимое
    setViewportMargins(0, m_headerHeight, 0, 0);
}

void SessionListView::paintEvent(QPaintEvent *event)
{
    // Сначала рисуем стандартный вид списка
    QListView::paintEvent(event);
    
    // Затем рисуем заголовок поверх
    QPainter painter(this);
    drawHeader(&painter);
}

void SessionListView::resizeEvent(QResizeEvent *event)
{
    QListView::resizeEvent(event);
    viewport()->update();
}

void SessionListView::drawHeader(QPainter *painter)
{
    // Настройки для рисования заголовка
    QRect headerRect(0, 0, width(), m_headerHeight);
    
    // Рисуем фон заголовка
    painter->fillRect(headerRect, QColor(240, 240, 240));
    
    // Рисуем границу снизу
    painter->setPen(QColor(208, 208, 208));
    painter->drawLine(0, m_headerHeight - 1, width(), m_headerHeight - 1);
    
    // Настройки шрифта
    QFont headerFont = font();
    headerFont.setBold(true);
    headerFont.setPointSize(10);
    painter->setFont(headerFont);
    painter->setPen(QColor(51, 51, 51));
    
    // Вычисляем ширину колонок (пропорции 2:2:1)
    int totalWidth = width();
    int nameWidth = totalWidth * 2 / 5;
    int dateWidth = totalWidth * 2 / 5;
    int messagesWidth = totalWidth * 1 / 5;
    
    // Рисуем текст заголовков
    QRect nameRect(5, 0, nameWidth - 10, m_headerHeight);
    QRect dateRect(nameWidth + 5, 0, dateWidth - 10, m_headerHeight);
    QRect messagesRect(nameWidth + dateWidth + 5, 0, messagesWidth - 10, m_headerHeight);
    
    painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, "Имя");
    painter->drawText(dateRect, Qt::AlignLeft | Qt::AlignVCenter, "Дата создания");
    painter->drawText(messagesRect, Qt::AlignLeft | Qt::AlignVCenter, "Количество сообщений");
}

