/**
 * @file context.cpp
 * @brief Определение класса Context
 */
#include "context.h"

Context &Context::get() {
    static Context context;
    return context;
}

void Context::run() {
    if (m_started) {
        m_context.restart();
    }
    m_started = true;

    // В версии Boost 1.88.0+ доступен только этот метод run, выбрасывающий
    // исключения
    try {
        m_context.run();
    }
    catch (...) {}
}

void Context::stop() {
    if (m_context.stopped()) {
        return;
    }

    m_context.stop();
}
