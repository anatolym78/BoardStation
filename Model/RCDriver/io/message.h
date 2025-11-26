/**
 * @file message.h
 * @brief Объявление типов объектов, описывающих сообщения с данными
 */
#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

/**
 * @brief Массив данных сообщения
 */
using MessageData = std::vector<uint8_t>;

/**
 * @brief Объект сообщения, хранящий константные данные
 */
using ConstMessage = std::shared_ptr<const MessageData>;

/**
 * @class Message
 * @brief Объект сообщения, хранящий неконстантные данные
 * @details Класс при помощи композиции добавляет функционал для типа
 *          std::shared_ptr<MessageData>.
 *
 *          Стандартная реализация std::shared_ptr не подразумевает
 *          наследования.
 */
class Message {
public:
    Message() = default;

    Message(const ConstMessage &msg);
    Message(const Message &msg);
    Message(Message &&msg);

    Message &operator=(const Message &msg);
    Message &operator=(Message &&msg);

    /**
     * @brief Создать объект сообщения
     * @param args Аргументы для конструктора типа MessageData
     * @return Созданный объект сообщения
     */
    template <typename... Args>
    static Message create(Args &&...args);

    /**
     * @brief Оператор приведения типа к сообщению, хранящему константные
     *        данные
     * @return Объект сообщения, хранящий константные данные
     */
    operator ConstMessage() const { return m_ptr; }

    /**
     * @brief Обращение к объекту данных сообщения через указатель
     * @return Указатель на массив данных сообщения
     */
    MessageData *operator->() const noexcept { return m_ptr.get(); }
    /**
     * @brief Обращение к объекту данных сообщения через ссылку
     * @return Ссылка на массив данных сообщения
     */
    MessageData &operator*() const { return *m_ptr; }

    /**
     * @brief Проверить валидность хранимого указателя
     * @return true, если хранимый указатель не нулевой
     */
    explicit operator bool() const noexcept { return static_cast<bool>(m_ptr); }

    /**
     * @brief Получить объект на хранимый указатель
     * @return Указатель на массив данных сообщения
     */
    MessageData *get() const noexcept { return m_ptr.get(); }

    /**
     * @brief Удалить текущий объект
     */
    void reset() noexcept { m_ptr.reset(); }

    /**
     * @brief Поменять данные сообщений между объектами сообщений
     * @param other Другой объект сообщения
     */
    void swap(Message &other) noexcept { m_ptr.swap(other.m_ptr); }

private:
    std::shared_ptr<MessageData> m_ptr;
};

template <typename... Args>
Message Message::create(Args &&...args) {
    Message msg;
    msg.m_ptr = std::make_shared<MessageData>(std::forward<Args>(args)...);
    return msg;
}
