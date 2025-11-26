/**
 * @file listener.h
 * @brief Объявление интерфейса наблюдателя
 */
#pragma once

#include <memory>

#include "subject.h"

/**
 * @class Listener
 * @brief Интерфейс наблюдателя
 * @details Интерфейс наблюдателя необходим для регистрации и обработки событий
 *          по мере их наступления
 */
class Listener {
public:
    virtual ~Listener() = default;

    /**
     * @brief Уведомить наблюдателя об изменении состояния
     * @param subject Объект наблюдения
     */
    virtual void update(const Subject &subject) = 0;
};

using Listener_Ptr = std::shared_ptr<Listener>;
using Listener_WeakPtr = std::weak_ptr<Listener>;
