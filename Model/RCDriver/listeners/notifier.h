/**
 * @file notifier.h
 * @brief Объявление класса ListenerNotifier
 */
#pragma once

#include <list>
#include <mutex>

#include "listener.h"
#include "listenermanager.h"
#include "subject.h"

/**
 * @class ListenerNotifier
 * @brief Контейнер наблюдателей с функцией уведомления
 */
class ListenerNotifier : public ListenerManager {
public:
    virtual ~ListenerNotifier() = default;

    /**
     * @brief Привязать наблюдателя
     * @param listener Новый наблюдатель
     */
    void attach(Listener_Ptr listener) override;
    /**
     * @brief Отвязать наблюдателя
     * @param listener Наблюдатель, которого необходимо отвязать
     */
    void detach(Listener_Ptr listener) override;

    /**
     * @brief Уведомить зарегистрированных наблюдателей об обновлении состояния
     *        объекта наблюдения
     * @warning Метод не проверяет хранящиеся указатели на валидность
     * @param subject Объект наблюдения
     */
    void notify(const Subject &subject);

protected:
    std::list<Listener_WeakPtr> m_listeners;
    std::mutex m_listeners_mtx;
};
