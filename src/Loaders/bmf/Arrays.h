#ifndef ARRAYS_H
#define ARRAYS_H

#include <string.h>
#include <algorithm>
#include <initializer_list>

#include "Defs.h"

/// Невладеющий контейнер массива
/// фиксированного рамера

template<typename _Data>
class FixedArray
{
    DISABLE_COPY(FixedArray);
    
protected:

    _Data *m_begin{ nullptr };
    _Data *m_end{ nullptr };
    
private:

    inline FixedArray(_Data *p_begin,
                      _Data *p_end) noexcept:
        m_begin(p_begin),
        m_end(p_end)
    {}

public:

    typedef _Data* Iterator;

    typedef const _Data* ConstIterator;

    FixedArray() = default;
    
/// Конструктор переноса

    inline FixedArray(FixedArray &&p_array) noexcept:
        FixedArray<_Data>(p_array.m_begin, p_array.m_end)
    {
        p_array.m_begin = p_array.m_end = nullptr;
    }

/// Вернуть итератор на начало массива

    inline Iterator begin() noexcept
    {
        return m_begin;
    }

/// Вернуть итератор на конец массива

    inline Iterator end() noexcept
    {
        return m_end;
    }
    
/// Вернуть константный итератор на начало массива

    inline ConstIterator begin() const noexcept
    {
        return m_begin;
    }    

/// Вернуть константный итератор на конец массива

    inline ConstIterator end() const noexcept
    {
        return m_end;
    }

/// Вернуть константный итератор на начало массива

    inline ConstIterator cbegin() const noexcept
    {
        return m_begin;
    }    

/// Вернуть константный итератор на конец массива

    inline ConstIterator cend() const noexcept
    {
        return m_end;
    }

/// Вернуть первый элемент

    inline const _Data& front() const
    {
        return *m_begin;
    }

/// Вернуть первый элемент

    inline _Data& front()
    {
        return *m_begin;
    }

/// Вернуть последний элемент

    inline const _Data& back() const
    {
        return *(m_end - 1);
    }

/// Вернуть последний элемент

    inline _Data& back()
    {
        return *(m_end - 1);
    }

/// Вернуть данные контейнера

    inline const _Data* data() const noexcept
    {
        return m_begin;
    }

/// Вернуть данные контейнера

    inline _Data* data() noexcept
    {
        return m_begin;
    }

/// Вернуть элемент

    inline const _Data& operator[](size_t p_pos) const noexcept
    {
        return m_begin[p_pos];
    }

/// Вернуть элемент
    
    inline _Data& operator[](size_t p_pos) noexcept
    {
        return m_begin[p_pos];
    }

/// Вернуть число элементов

    inline size_t size() const noexcept
    {
        return static_cast<size_t>(m_end - m_begin);
    }

/// Проверка на пустоту контейнера

    inline bool empty() const noexcept
    {
        return m_begin == m_end;
    }

/// Заполнить массив заданным элементом

    inline void fill(const _Data &p_element) noexcept
    {
        std::fill(m_begin, m_end, p_element);
    }

/// Заполнить массив заданным элементом

    inline void fill(size_t       p_begin,
                     size_t       p_end,
                     const _Data &p_element) noexcept
    {
        std::fill(m_begin + p_begin, m_begin + p_end, p_element);
    }

/// Вернуть отрезок константного массива со сдвигом
/// от левой границы

    inline FixedArray<_Data> left(size_t p_begin_offset) const noexcept
    {
        if (p_begin_offset >= static_cast<size_t>(m_end - m_begin))
            return{};

        return FixedArray<_Data>(m_begin + p_begin_offset,
                                 m_end);
    }

/// Вернуть отрезок константного массива со сдвигом
/// от правой границы

    inline FixedArray<_Data> right(size_t p_end_offset) const noexcept
    {
        if (p_end_offset >= static_cast<size_t>(m_end - m_begin))
            return{};

        return FixedArray<_Data>(m_begin,
                                 m_end - p_end_offset);
    }

/// Вернуть отрезок константного массива

    inline FixedArray<_Data> mid(size_t p_start, size_t p_end) const noexcept
    {
        if (p_end <= p_start || p_end > static_cast<size_t>(m_end - m_begin))
            return{};

        return FixedArray<_Data>(m_begin + p_start,
                                 m_begin + p_end);
    }

}; // class FixedArray

/// Контейнер массива

template<typename _Data>
class Array final : public FixedArray<_Data>
{
    Array(const Array&) = delete;

    using FixedArray<_Data>::m_begin;
    using FixedArray<_Data>::m_end;

public:

    Array() = default;

/// Конструктор переноса

    inline Array(Array &&p_array) noexcept:
          FixedArray<_Data>::FixedArray(std::move(p_array))
    {}
   
/// Конструктор по заданному размеру

    inline Array(size_t p_size)
    {
        resize(p_size);
    }

/// Конструктор по std::initializer_list

    inline Array(std::initializer_list<_Data> p_array)
    {
        resize(p_array.size());
        std::copy(p_array.begin(), p_array.end(), m_begin);
    }

/// Отчистка контейнера

    inline void clear()
    {
        delete[] m_begin;
        m_begin = nullptr;
        m_end = nullptr;
    }

/// Изменить размер

    void resize(size_t p_new_size = 0,
                bool   p_copy_data = false)
    {
        const size_t current_size = static_cast<size_t>(m_end - m_begin);

        if (p_new_size == current_size)
            return;

        if (p_new_size == 0) {
            clear();
            return;
        }

        _Data *new_data = new _Data[p_new_size];

        m_end = new_data + p_new_size;

        if (current_size == 0)
            m_begin = new_data;
        else {
            if (p_copy_data) {
                size_t copied_memory_size = (mat::min)(current_size, p_new_size);
                std::copy_n(m_begin, copied_memory_size, new_data);
            }

            delete[] m_begin;
            m_begin = new_data;
        }
    }

    inline ~Array()
    {
        clear();
    }

/// Оператор переноса

    inline void operator =(Array &&p_array) noexcept
    {
        m_begin = p_array.m_begin;
        m_end = p_array.m_end;

        p_array.m_begin = nullptr;
        p_array.m_end = nullptr;
    }

/// Оператор копирования из FixedArray<_Data>

    inline void operator =(const FixedArray<_Data> &p_array)
    {
        resize(p_array.size());
        std::copy(p_array.cbegin(), p_array.cend(), m_begin);
    }

/// Оператор копирования из Array<_Data>

    inline void operator =(const Array<_Data> &p_array)
    {
        operator=(static_cast<const FixedArray<_Data>&>(p_array));
    }

/// Оператор копирования из std::initializer_list

    inline void operator =(std::initializer_list<_Data> p_array)
    {
        resize(p_array.size());
        std::copy(p_array.begin(), p_array.end(), m_begin);
    }
    
/// Оператор присваивания одного элемента

    inline void operator =(const _Data &p_value)
    {
        resize(1);
        *m_begin = p_value;
    }
}; // class Array

#endif // ARRAY_H
