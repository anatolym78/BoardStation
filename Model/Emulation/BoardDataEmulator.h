#ifndef BOARDDATAEMULATOR_H
#define BOARDDATAEMULATOR_H

#include "../IDriver.h"
#include "BoardDataGenerators/ParameterGenerator.h"
#include "../JsonWriter.h"
#include <QTimer>
#include <QQueue>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <cmath>

namespace drv {

class BoardDataEmulator : public IDriver {
    Q_OBJECT

public:
    explicit BoardDataEmulator(QObject *parent = nullptr);
    ~BoardDataEmulator() override;

    // IDriver interface implementation
    void write(const std::string &data) override;
    const std::string &read() override;
    void flushData() override;

    // Control methods
    void start();
    void stop();

private slots:
    void onStateTimerTimeout();
    void onDataTimerTimeout();

private:
    void setupGenerators();
    void generateParameters();

private:
    QTimer *m_stateTimer;
    QTimer *m_dataTimer;
    QQueue<std::string> m_messageQueue;
    
    // Генераторы параметров
    QList<ParameterGenerator*> m_generators;
    
    // JSON writer для создания JSON строк
    JsonWriter *m_jsonWriter;
    
    // Время для генерации параметров
    double m_time;
    
    State m_currentState;
    bool m_isRunning;
};

} // namespace drv

#endif // BOARDDATAEMULATOR_H
