/**
 * @file context.h
 * @brief Объявление объекта, управляющего асинхронными операциями и их
 *        выполнением
 */
#pragma once

#include "boost/asio/io_context.hpp"

/**
 * @class Context
 * @brief Объект управления асинхронными операциями ввода/вывода и их
 *        выполнения (контекст)
 * @warning Класс реализует паттерн одиночки. Нельзя создавать объект этого
 *          класса, необходимо использовать статический метод get.
 */
class Context {
public:
    ~Context() { stop(); }

    Context(const Context &other) = delete;
    Context(Context &&other) = delete;

    Context &operator=(const Context &other) = delete;
    Context &operator=(Context &&other) = delete;

    /**
     * @brief Получить ссылку на объект контекста
     * @return Ссылка на объект контекста
     */
    static Context &get();

    /**
     * @brief Запустить выполнение асинхронных операций
     * @details Метод блокируется до принудительного или аварийного завершения
     *          работы контекста
     */
    void run();
    /**
     * @brief Завершить выполнение асинхронных операций
     */
    void stop();

    /**
     * @brief Оператор приведения типа объекта к boost::asio::io_context
     * @return Ссылка на объект типа boost::asio::io_context
     */
    operator boost::asio::io_context &() { return m_context; }

private:
    Context() = default;

    boost::asio::io_context m_context;
    bool m_started = false;
};
