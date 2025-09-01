#include "AltitudeGenerator.h"
#include <QDebug>

AltitudeGenerator::AltitudeGenerator(QObject *parent)
    : ParameterGenerator(parent)
    , m_baseAltitude(500.0)
    , m_amplitude(2.0)
    , m_period(5.0)
{
}

Parameter AltitudeGenerator::generate(double time)
{
    // Генерируем высоту по синусоидальному закону
    double altitude = m_baseAltitude + m_amplitude * sin(2.0 * M_PI * time / m_period);
    
    // Создаем параметр с названием, значением и единицей измерения
    Parameter param(tr("Altitude"), QString::number(altitude, 'f', 2), tr("m"));
    
    return param;
}

QString AltitudeGenerator::getName() const
{
    return "AltitudeGenerator";
}

void AltitudeGenerator::setBaseAltitude(double baseAltitude)
{
    m_baseAltitude = baseAltitude;
}

void AltitudeGenerator::setAmplitude(double amplitude)
{
    m_amplitude = amplitude;
}

void AltitudeGenerator::setPeriod(double period)
{
    m_period = period;
}
