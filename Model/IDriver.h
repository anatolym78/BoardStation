#ifndef IDRIVER_H
#define IDRIVER_H

#include <string>
#include <QObject>

namespace drv {

// A driver interface
class IDriver : public QObject {
    Q_OBJECT

public:
    enum class State {
        k_Connected,
        k_Disconnected
    };

    virtual ~IDriver() = default;

    virtual void write(const std::string &data) = 0;
    virtual const std::string &read() = 0;
    virtual void flushData() = 0;

signals:
    void dataAvailable();
    void stateChanged(State state);

protected:
    void emitDataAvailable() { emit dataAvailable(); }
    void emitStateChanged(State state) { emit stateChanged(state); }
};

} // namespace drv

#endif // IDRIVER_H
