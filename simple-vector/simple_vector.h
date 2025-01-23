#pragma once
#include <cassert>
#include <initializer_list>
#include<memory>
#include<stdexcept>
#include<utility>


class ReserveProxyObj
{
public:
    ReserveProxyObj(size_t temp) :c(temp) {};

    size_t c = 0;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
};


template <typename Type>
class SimpleVector {
public:

    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;


    explicit SimpleVector(size_t size)
    {
        // Создаёт вектор из size элементов, инициализированных значением по умолчанию
        if (size == 0)
        {
            vector_size = 0;
        }
        else
        {
            ptr = new Type[size]();
        }
        vector_size = size;
        capacity = size;

    }
    SimpleVector(SimpleVector&& other)
    {
        vector_size = other.vector_size;
        capacity = other.capacity;
        ptr = other.ptr;
        other.vector_size = 0;
        other.ptr = nullptr;
    }
    SimpleVector(ReserveProxyObj proxy)
    {
        capacity = proxy.c;
        ptr = new Type[capacity];
    }

    SimpleVector(size_t size, const Type& value)
    {
        // Создаёт вектор из size элементов, инициализированных значением value
        ptr = new Type[size];
        for (size_t i = 0; i < size; ++i)
        {
            *(ptr + i) = value;
        }
        vector_size = size;
        capacity = size;
    }



    SimpleVector(std::initializer_list<Type> init)
    {// Создаёт вектор из std::initializer_list
        size_t size = init.size();
        ptr = new Type[size];
        Type* temp_ptr = ptr;
        for (const auto& ch : init)
        {
            *temp_ptr = ch;
            ++temp_ptr;
        }
        vector_size = size;
        capacity = vector_size;
    }

    SimpleVector(const SimpleVector& other)
    {
        size_t size = other.vector_size;
        ptr = new Type[size];
       
        for (size_t i = 0; i < size; ++i)
        {
            *(ptr + i) = *(std::move(other.ptr + i));
        }
        vector_size = size;
        capacity = size;
    }

    SimpleVector& operator=(const SimpleVector& rhs)
    {
        if (rhs.ptr == this->ptr)
        {
            return *this;
        }
        size_t size = rhs.vector_size;
        Type* new_ptr = new Type[size];
        for (size_t i = 0; i < size; ++i)
        {
            *(new_ptr + i) = *(rhs.ptr + i);
        }
        delete[] ptr;
        ptr = new_ptr;
        vector_size = size;
        capacity = size;
        return *this;
    }

   SimpleVector& operator=(SimpleVector&& rhs)
   {
       ptr = rhs.ptr;
       vector_size = rhs.vector_size;
       capacity = rhs.capacity;
       rhs.ptr = nullptr;
       rhs.vector_size = 0;
        return *this;
    }

    void Reserve(size_t new_capacity)
    {
        if (new_capacity < capacity)
        {
            return;
        }
        else if (this->ptr == nullptr)
        {
            capacity = new_capacity;
            ptr = new Type[capacity];
            return;
        }
        Type* new_ptr = new Type[new_capacity];
        std::copy(this->begin(), this->end(), new_ptr);
        capacity = new_capacity;
        delete[] ptr;
        ptr = new_ptr;
        return;
    }

    void PushBack(const Type& item)
    {
        // Добавляет элемент в конец вектора
       // При нехватке места увеличивает вдвое вместимость вектора

        if (vector_size < capacity)
        {
            ptr[vector_size] = item;
            ++vector_size;
        }
        else
        {
            size_t temp = capacity == 0 ? 1 : capacity * 2;
            Type* new_ptr = new Type[temp];
            for (size_t i = 0; i < vector_size; ++i)
            {
                new_ptr[i] = ptr[i];
            }
            new_ptr[vector_size] = item;
            delete[]ptr;
            ptr = new_ptr;
            capacity = temp;
            ++vector_size;

        }

    }

    void PushBack(Type&& item)
    {
        // Добавляет элемент в конец вектора
       // При нехватке места увеличивает вдвое вместимость вектора

        if (vector_size < capacity)
        {
            ptr[vector_size] = std::move(item);
            ++vector_size;
        }
        else
        {
            size_t temp = capacity == 0 ? 1 : capacity * 2;
            Type* new_ptr = new Type[temp];
            for (size_t i = 0; i < vector_size; ++i)
            {
                new_ptr[i] =std::move(ptr[i]);
            }
            new_ptr[vector_size] =std::move(item);
            delete[]ptr;
            ptr = new_ptr;
            capacity = temp;
            ++vector_size;

        }

    }



    Iterator Insert(ConstIterator pos, const Type& value)
    {
        // Вставляет значение value в позицию pos.
        // Возвращает итератор на вставленное значение
        // Если перед вставкой значения вектор был заполнен полностью,
        // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
        Type* posi = const_cast<Type*>(pos);
        Type* position = nullptr;
        if (vector_size < capacity)
        {
            std::copy_backward(posi, this->end(), this->end() + 1);
            *posi = std::move(value);
            ++vector_size;
            position = posi;
        }

        else if (vector_size == capacity)
        {
            size_t dest_from_begin = posi - this->begin();
            size_t capacity_temp = capacity == 0 ? 1 : capacity * 2;
            Type* new_ptr = new Type[capacity_temp];
            std::copy(this->begin(), posi, new_ptr);
            position = new_ptr + dest_from_begin;
            *(position) =std::move( value);
            std::copy(posi, this->end(), (position + 1));
            delete[] ptr;
            ptr = new_ptr;
            ++vector_size;
            capacity = capacity_temp;
        }
        return position;
    }

    Iterator Insert(ConstIterator pos,  Type&& value)
    {
        
        Type* posi = const_cast<Type*>(pos);
        Type* position = nullptr;
        if (vector_size < capacity)
        {
            std::move_backward(posi, this->end(), this->end() + 1);
            *posi = std::move(value);
            ++vector_size;
            position = posi;
        }

        else if (vector_size == capacity)
        {
            size_t dest_from_begin = posi - this->begin();
            size_t capacity_temp = capacity == 0 ? 1 : capacity * 2;
            Type* new_ptr = new Type[capacity_temp];
            std::move(this->begin(), posi, new_ptr);
            position = new_ptr + dest_from_begin;
            *(position) = std::move(value);
            std::move(posi, this->end(), (position + 1));
            delete[] ptr;
            ptr = new_ptr;
            ++vector_size;
            capacity = capacity_temp;
        }
        return position;
    }

    void PopBack() noexcept
    {
        if (vector_size == 0)
        {
            return;
        }
        --vector_size;
    }

    Iterator Erase(ConstIterator pos)
    {// Удаляет элемент вектора в указанной позиции

        Type* pos_temp = const_cast<Type*>(pos);
        std::move((pos_temp + 1), this->end(), pos_temp);
        --vector_size;
        return pos_temp;
    }

    void swap(SimpleVector& other) noexcept
    {// Обменивает значение с другим вектором

        std::swap(this->ptr, other.ptr);
        std::swap(this->vector_size, other.vector_size);
        std::swap(this->capacity, other.capacity);
    }

    ~SimpleVector()
    {

        
        delete[] ptr;

    }

    size_t GetSize() const noexcept
    {// Возвращает количество элементов в массиве

        return vector_size;
    }


    size_t GetCapacity() const noexcept
    {// Возвращает вместимость массива
        return capacity;
    }


    bool IsEmpty() const noexcept
    {
        return vector_size == 0;
    }


    Type& operator[](size_t index) noexcept
    {// Возвращает ссылку на элемент с индексом index
        return *(ptr + index);
    }


    const Type& operator[](size_t index) const noexcept
    {// Возвращает константную ссылку на элемент с индексом index
        return *(ptr + index);
    }


    Type& At(size_t index)
    {
        // Возвращает константную ссылку на элемент с индексом index
        // Выбрасывает исключение std::out_of_range, если index >= size
        if (index >= vector_size)
        {
            throw std::out_of_range("out_of_range");
        }
        return *(ptr + index);

    }


    const Type& At(size_t index) const
    {
        // Возвращает константную ссылку на элемент с индексом index
       // Выбрасывает исключение std::out_of_range, если index >= size
        if (index >= vector_size)
        {
            throw std::out_of_range("out_of_range");
        }
        return *(ptr + index);

    }


    void Clear() noexcept
    {
        // Обнуляет размер массива, не изменяя его вместимость
        vector_size = 0;
        return;
    }


    void Resize(size_t new_size)
    {
        // Изменяет размер массива.
       // При увеличении размера новые элементы получают значение по умолчанию для типа Type
        if (new_size <= vector_size)
        {

        }
        else if (new_size <= capacity)
        {
            for (size_t i = vector_size; i < new_size; ++i)
            {
                Type temp{};
                *(ptr + i) = std::move(temp);
            }

        }
        else if (new_size > capacity)
        {
            Type* new_ptr = new Type[new_size]();
            for (size_t i = 0; i < vector_size; ++i)
            {
                *(new_ptr + i) = std::move(*(ptr + i));
            }
            delete[] ptr;
            ptr = new_ptr;
            capacity = new_size;

        }
        vector_size = new_size;

    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept
    {
        return ptr;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept
    {
        return ptr + vector_size;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept
    {
        return ptr;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept
    {
        return ptr + vector_size;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept
    {
        return ptr;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept
    {
        return ptr + vector_size;
    }



private:
    Type* ptr = nullptr;
    size_t vector_size = 0;
    size_t capacity = 0;
};


template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{

    return !std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{

    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    bool result = std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end()) ||
        std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    return result;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    bool result = lhs <= rhs;
    return !result;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    bool result = lhs < rhs;
    return !result;
}
