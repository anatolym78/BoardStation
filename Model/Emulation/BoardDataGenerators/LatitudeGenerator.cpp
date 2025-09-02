#include "LatitudeGenerator.h"
#include <QDebug>

LatitudeGenerator::LatitudeGenerator(QObject *parent)
    : ParameterGenerator(parent)
    , m_startLatitude(55.7558)  // Начальная широта (Москва)
    , m_speed(0.0005)           // Скорость изменения (медленное движение)
{
}

BoardParameter LatitudeGenerator::generate(double time)
{
    // Генерируем широту по линейному закону
    double latitude = m_startLatitude + m_speed * time;
    
    // Создаем параметр с названием, значением и единицей измерения
    BoardParameter param(tr("Latitude"), QString::number(latitude, 'f', 6), tr("°"));
    
    return param;
}

QString LatitudeGenerator::getName() const
{
    return "LatitudeGenerator";
}

void LatitudeGenerator::setStartLatitude(double startLatitude)
{
    m_startLatitude = startLatitude;
}

void LatitudeGenerator::setSpeed(double speedDegreesPerSecond)
{
    m_speed = speedDegreesPerSecond;
}
