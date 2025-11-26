/**
 * @file message.cpp
 * @brief Определение класса Message
 */
#include "message.h"

#include <cstring>

Message::Message(const ConstMessage &msg) {
    if (!msg) {
        return;
    }

    m_ptr = std::make_shared<MessageData>(msg->size());
    std::memcpy(m_ptr->data(), msg->data(), msg->size());
}

Message::Message(const Message &msg) : m_ptr(msg.m_ptr) {}

Message::Message(Message &&msg) : m_ptr(std::move(msg.m_ptr)) {}

Message &Message::operator=(const Message &msg) {
    m_ptr = msg.m_ptr;
    return *this;
}

Message &Message::operator=(Message &&msg) {
    m_ptr.swap(msg.m_ptr);
    return *this;
}
