#include "LongitudeGenerator.h"
#include <QDebug>

LongitudeGenerator::LongitudeGenerator(QObject *parent)
    : ParameterGenerator(parent)
    , m_startLongitude(37.6173)  // Начальная долгота (Москва)
    , m_speed(0.001)             // Скорость изменения (медленное движение)
{
}

BoardParameter* LongitudeGenerator::generate(double time)
{
    // Генерируем долготу по линейному закону
    double longitude = m_startLongitude + m_speed * time;
    
    // Создаем параметр с названием, значением и единицей измерения
    BoardParameter *param = new BoardParameter(tr("Longitude"), QString::number(longitude, 'f', 6), tr("°"));
    
    return param;
}

QString LongitudeGenerator::getName() const
{
    return "LongitudeGenerator";
}

void LongitudeGenerator::setStartLongitude(double startLongitude)
{
    m_startLongitude = startLongitude;
}

void LongitudeGenerator::setSpeed(double speedDegreesPerSecond)
{
    m_speed = speedDegreesPerSecond;
}
