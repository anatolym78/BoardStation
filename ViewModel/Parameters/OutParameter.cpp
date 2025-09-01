#include "OutParameter.h"

OutParameter::OutParameter(const QString &label, const QString &controlType)
    : QObject()
    , m_label(label)
    , m_widget(nullptr)
    , m_controlType(controlType)
{
}
