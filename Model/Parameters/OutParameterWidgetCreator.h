#ifndef OUTPARAMETERWIDGETCREATOR_H
#define OUTPARAMETERWIDGETCREATOR_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QList>

// Forward declarations
class OutParameter;
class RealOutParameter;
class ListedRealOutParameter;
class RangedRealOutParameter;
class StringOutParameter;
class BooleanOutParameter;

class OutParameterWidgetCreator : public QObject
{
    Q_OBJECT

public:
    explicit OutParameterWidgetCreator(QObject *parent = nullptr);
    
    // Основной метод для создания виджета управления
    QWidget* createControlWidget(OutParameter *parameter);
    
    // Специализированные методы для каждого типа параметра
    QWidget* createWidgetForListedReal(ListedRealOutParameter *parameter);
    QWidget* createWidgetForRangedReal(RangedRealOutParameter *parameter);
    QWidget* createWidgetForString(StringOutParameter *parameter);
    QWidget* createWidgetForBoolean(BooleanOutParameter *parameter);
    QWidget* createWidgetForReal(RealOutParameter *parameter);

private:
    // Вспомогательные методы
    QWidget* createWidgetContainer(QWidget *widget);
    void connectValueChangedSignal(QWidget *widget, OutParameter *parameter);
};

#endif // OUTPARAMETERWIDGETCREATOR_H
