#include "SpeedGenerator.h"
#include <QDebug>

SpeedGenerator::SpeedGenerator(QObject *parent)
    : ParameterGenerator(parent)
    , m_baseSpeed(50.0)         // Базовая скорость 50 м/с
    , m_amplitude(5.0)          // Амплитуда колебаний ±5 м/с
    , m_period(3.0)             // Период колебаний 3 секунды
{
}

Parameter SpeedGenerator::generate(double time)
{
    // Генерируем скорость с колебаниями по синусоидальному закону
    double speed = m_baseSpeed + m_amplitude * sin(2.0 * M_PI * time / m_period);
    
    // Создаем параметр с названием, значением и единицей измерения
    Parameter param(tr("Speed"), QString::number(speed, 'f', 1), tr("m/s"));
    
    return param;
}

QString SpeedGenerator::getName() const
{
    return "SpeedGenerator";
}

void SpeedGenerator::setBaseSpeed(double baseSpeed)
{
    m_baseSpeed = baseSpeed;
}

void SpeedGenerator::setAmplitude(double amplitude)
{
    m_amplitude = amplitude;
}

void SpeedGenerator::setPeriod(double period)
{
    m_period = period;
}
