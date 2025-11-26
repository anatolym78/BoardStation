/**
 * @file messagelistener.h
 * @brief Объявления интерфейса MessageListener
 */
#pragma once

#include "io/message.h"
#include "listener.h"

/**
 * @class MessageListener
 * @brief Интерфейс наблюдателя за входящими сообщениями
 * @details Интерфейс обновляется каждый раз, когда появляются новые
 *          входящие данные
 */
class MessageListener : public Listener {
public:
    virtual ~MessageListener() = default;

    /**
     * @brief Обработать входящее сообщение
     * @param status Данные нового входящего сообщения
     */
    virtual void messageReceived(ConstMessage message) = 0;

private:
    /**
     * @brief Уведомить наблюдателя получении данных нового сообщения
     * @warning Объект наблюдения @p subject иметь один из следующих типов:
                const SubjectContainer<Message> &,
                const SubjectContainer<ConstMessage> &
     * @param subject Объект наблюдения
     */
    void update(const Subject &subject) override;
};
