/**
 * @file listenermanager.h
 * @brief Объявление интерфейса менеджера наблюдателей
 */
#pragma once

#include <memory>

#include "listener.h"

/**
 * @class ListenerManager
 * @brief Интерфейс менеджера наблюдателей
 * @details Менеджер хранит указатели на объекты наблюдателей
 */
class ListenerManager {
public:
    virtual ~ListenerManager() = default;

    /**
     * @brief Привязать наблюдателя
     * @param listener Новый наблюдатель
     */
    virtual void attach(Listener_Ptr listener) = 0;
    /**
     * @brief Отвязать наблюдателя
     * @param listener Наблюдатель, которого необходимо отвязать
     */
    virtual void detach(Listener_Ptr listener) = 0;
};

using ListenerManager_Ptr = std::shared_ptr<ListenerManager>;
using ListenerManager_WeakPtr = std::weak_ptr<ListenerManager>;
