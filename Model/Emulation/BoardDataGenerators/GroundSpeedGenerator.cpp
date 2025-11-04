#include "GroundSpeedGenerator.h"

GroundSpeedGenerator::GroundSpeedGenerator(QObject *parent)
    : ParameterGenerator(parent)
    , m_averageSpeed(50.0)
    , m_amplitude(5.0)
    , m_period(5.0)
{
}

QString GroundSpeedGenerator::getName() const
{
    return "telemetry.groundSpeed";
}

BoardParameterSingle* GroundSpeedGenerator::generate(double time)
{
    double value = m_averageSpeed + m_amplitude * std::sin(2 * M_PI * time / m_period);
    return new BoardParameterSingle(getName(), QVariant::fromValue(value));
}
