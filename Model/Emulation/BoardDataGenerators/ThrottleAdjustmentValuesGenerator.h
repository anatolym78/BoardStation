#ifndef THROTTLEADJUSTMENTVALUESGENERATOR_H
#define THROTTLEADJUSTMENTVALUESGENERATOR_H

#include "ParameterGenerator.h"
#include <QVector>
#include <cmath>
#include "Model/Parameters/BoardParameterSingle.h"

class ThrottleAdjustmentValuesGenerator : public ParameterGenerator
{
    Q_OBJECT

public:
    explicit ThrottleAdjustmentValuesGenerator(QObject *parent = nullptr);
    BoardParameterSingle* generate(double time) override;
    QString getName() const override;

    void setupThrottleParameter(int index, double amplitude, double period, double phase);

private:
    struct SinusoidParams
    {
        double amplitude = 1.0;
        double period = 10.0;
        double phase = 0.0;
    };

    QVector<SinusoidParams> m_params;
};

#endif // THROTTLEADJUSTMENTVALUESGENERATOR_H
