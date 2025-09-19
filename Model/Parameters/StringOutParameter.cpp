#include "StringOutParameter.h"
#include <QString>

StringOutParameter::StringOutParameter(const QString &label, 
                                     const int &valueIndex,
                                     const QStringList &valuesList,
                                     const QString &controlType)
    : OutParameter(label, controlType)
    , m_valueIndex(valueIndex)
    , m_valuesList(valuesList)
{
}

QVariant StringOutParameter::getValue() const
{
    if (m_valueIndex >= 0 && m_valueIndex < m_valuesList.count()) return m_valuesList[m_valueIndex];

    return {};
}

void StringOutParameter::setValueIndex(int valueIndex)
{
	if (valueIndex >= 0 && valueIndex < m_valuesList.count()) return;

    m_valueIndex = valueIndex;
	
}

void StringOutParameter::setValuesList(const QStringList& valuesList)
{
	m_valuesList = valuesList;
}

bool StringOutParameter::isValid() const
{
    // Для строкового параметра допустим только QTextEdit
    return m_controlType == "QTextEdit";
}

