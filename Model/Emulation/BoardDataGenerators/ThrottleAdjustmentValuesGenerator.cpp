#include "ThrottleAdjustmentValuesGenerator.h"
#include <QVariantList>
#include "Model/Parameters/BoardParameterSingle.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ThrottleAdjustmentValuesGenerator::ThrottleAdjustmentValuesGenerator(QObject *parent)
    : ParameterGenerator(parent)
{
    m_params.resize(4);
}

BoardParameterSingle* ThrottleAdjustmentValuesGenerator::generate(double time)
{
    QVariantList values;
    for(const auto& p : m_params)
    {
        double value = p.amplitude * sin(2 * M_PI * time / p.period + p.phase);
        values.append(value);
    }

    return new BoardParameterSingle("stabData.throttleAdjustmentValues", values, QDateTime::currentDateTime(), "", this);
}

QString ThrottleAdjustmentValuesGenerator::getName() const
{
    return "stabData.throttleAdjustmentValues";
}

void ThrottleAdjustmentValuesGenerator::setupThrottleParameter(int index, double amplitude, double period, double phase)
{
    if (index >= 0 && index < m_params.size())
    {
        m_params[index].amplitude = amplitude;
        m_params[index].period = period;
        m_params[index].phase = phase;
    }
}
