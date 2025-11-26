/**
 * @file subjectcontainer.h
 * @brief Объявление класса SubjectContainer
 */
#pragma once

#include <utility>

#include "subject.h"

/**
 * @class SubjectContainer
 * @brief Реализация интерфейса объекта наблюдения, хранящего данные
 *        шаблонного типа
 * @details Класс предоставляет привычный функционал конструктора, присвоения,
 *          чтения и неявного преобразованя к типу данных объекта наблюдения
 */
template <typename DataType>
class SubjectContainer : public Subject {
public:
    SubjectContainer() = default;
    /**
     * @brief Конструктор копирования
     * @param data Объект, данные которого необходимо скопировать
     */
    SubjectContainer(const DataType &data) : m_data(data) {}
    /**
     * @brief Конструктор перемещения
     * @param data Объект, данные которого необходимо переместить
     */
    SubjectContainer(DataType &&data) : m_data(std::move(data)) {}

    /**
     * @brief Оператор присваивания с копированием
     * @param data Объект, данные которого необходимо скопировать
     */
    SubjectContainer<DataType> &operator=(const DataType &data) {
        m_data = data;
        return *this;
    }
    /**
     * @brief Оператор присваивания с перемещением
     * @param data Объект, данные которого необходимо переместить
     */
    SubjectContainer<DataType> &operator=(DataType &&data) {
        m_data = std::move(data);
        return *this;
    }

    /**
     * @brief Оператор обращения к объекту через ссылку
     * @return Ссылка на хранимый объект
     */
    DataType &operator*() noexcept { return m_data; }
    /**
     * @brief Оператор обращения к объекту через константную ссылку
     * @return Константная ссылка на хранимый объект
     */
    const DataType &operator*() const noexcept { return m_data; }

    /**
     * @brief Оператор обращения к объекту через указатель
     * @return Указатель на хранимый объект
     */
    DataType *operator->() noexcept { return &m_data; }
    /**
     * @brief Оператор обращения к объекту через константный указатель
     * @return Константный указатель на хранимый объект
     */
    const DataType *operator->() const noexcept { return &m_data; }

    /**
     * @brief Оператор преобразования к типу объекта наблюдения
     * @return Возвращает ссылку на объект наблюдения
     */
    operator DataType &() noexcept { return m_data; }
    /**
     * @brief Константный оператор преобразования к типу объекта наблюдения
     * @return Возвращает константную ссылку на объект наблюдения
     */
    operator const DataType &() const noexcept { return m_data; }

private:
    DataType m_data;
};
