/**
 * @file datalistener.h
 * @brief Объявления интерфейса наблюдателя за обновлением шаблонных данных
 */
#pragma once

#include <string>
#include <QDebug>

#include "listener.h"
#include "subjectcontainer.h"

/**
 * @class DataListener
 * @brief Интерфейс наблюдателя за обновлением шаблонных данных
 * @details Интерфейс обновляется каждый раз, когда содержимое шаблонных данных
 *          изменяется
 */
template <typename Data>
class DataListener : public Listener {
public:
    virtual ~DataListener() = default;

    /**
     * @brief Обработать новые данные
     * @param data Новые данные
     */
    virtual void dataUpdated(const Data &data) = 0;

private:
    /**
     * @brief Уведомить наблюдателя об изменении режима работы
     * @warning Объект наблюдения @p subject должен иметь тип
                const SubjectContainer<Data> &
     * @param subject Объект наблюдения
     */
    void update(const Subject &subject) override;
};

template <typename Data>
void DataListener<Data>::update(const Subject &subject) {
    static const std::string k_ModuleName = "DataListener";

    if (auto dataPtr = dynamic_cast<const SubjectContainer<Data> *>(&subject)) {
        dataUpdated(*dataPtr);
    }
    else {
        qDebug() << QString::fromStdString(k_ModuleName) + "::update: Unexpected subject type";
    }
}
