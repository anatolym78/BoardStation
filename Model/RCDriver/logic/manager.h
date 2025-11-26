/**
 * @file manager.h
 * @brief Объявление интерфейса менеджера (диспетчера)
 */
#pragma once

/**
 * @class Manager
 * @brief Интерфейс менеджера (диспетчера)
 */
class Manager {
public:
    virtual ~Manager() = default;

    /**
     * @brief Начать работу менеджера
     * @return true, если менеджер был успешно запущен
     */
    virtual bool start() = 0;
    /**
     * @brief Прекратить работу менеджера
     */
    virtual void stop() = 0;
};
