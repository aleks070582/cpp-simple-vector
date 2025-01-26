
#pragma once
#include <cassert>
#include <initializer_list>
#include<memory>
#include<stdexcept>
#include<utility>
#include"array_ptr.h"



class ReserveProxyObj {
public:
    ReserveProxyObj(size_t temp) : c(temp) {}

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

    SimpleVector() = default;

    explicit SimpleVector(size_t size) : vector_size(size), capacity(size)
    {
        ptr = ArrayPtr<Type>(size);
        Type temp{};
        std::fill(this->begin(), this->end(), temp);
    }

    SimpleVector(SimpleVector&& other) noexcept
      : ptr(std::move(other.ptr)), vector_size(other.vector_size), capacity(other.capacity) 
    {
        other.vector_size = 0;
        other.capacity = 0;
    }

    SimpleVector(ReserveProxyObj proxy) : vector_size(0), capacity(proxy.c) {
        ptr = ArrayPtr<Type>(capacity);
    }

    SimpleVector(size_t size, const Type& value) : SimpleVector(size)
    {
        
        std::fill(this->begin(), this->end(), value);
    }

    SimpleVector(std::initializer_list<Type> init):SimpleVector(init.size())
    {
       std::copy(init.begin(), init.end(), this->begin());
    }

    SimpleVector(const SimpleVector& other): SimpleVector(other.vector_size)
    {
          std::copy(other.begin(), other.end(), this->begin());
    }

    SimpleVector& operator=(const SimpleVector& rhs)
 {
        if (rhs.ptr.Get() == this->ptr.Get())
        {
            return *this;
        }
        SimpleVector temp(rhs);
        std::swap(*this, temp);
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        if (rhs.ptr.Get() == this->ptr.Get() )
        {
            return *this;
        }
        ptr = std::move(rhs.ptr);
        vector_size = rhs.vector_size;
        capacity = rhs.capacity;
        rhs.vector_size = 0;
        rhs.capacity = 0;
        return *this;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity) {
            return;
        }
        ArrayPtr<Type> new_ptr(new_capacity);
        std::move(this->begin(), this->end(), new_ptr.Get());
        capacity = new_capacity;
        ptr = std::move(new_ptr);
    }

    void PushBack(const Type& item) {
        if (vector_size < capacity) {
            ptr[vector_size] = item;
            ++vector_size;
        }
        else {
            size_t temp = capacity == 0 ? 1 : capacity * 2;
            this->Reserve(temp);
            ptr[vector_size] = item;
            ++vector_size;
        }
    }

    void PushBack(Type&& item) {
        if (vector_size < capacity) {
            ptr[vector_size] = std::move(item);
            ++vector_size;
        }
        else {
            size_t temp = capacity == 0 ? 1 : capacity * 2;
            this->Reserve(temp);
            ptr[vector_size] = std::move(item);
            ++vector_size;
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t position = pos - this->begin();
        if (vector_size == capacity) {
            size_t capacity_temp = capacity == 0 ? 1 : capacity * 2;
            this->Reserve(capacity_temp);
        }
        std::copy_backward(this->begin() + position, this->end(), this->end() + 1);
        *(this->begin() + position) = value;
        ++vector_size;
        return this->begin() + position;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        size_t position = pos - this->begin();
        if (vector_size == capacity) {
            size_t capacity_temp = capacity == 0 ? 1 : capacity * 2;
            this->Reserve(capacity_temp);
        }
        std::move_backward(this->begin() + position, this->end(), this->end() + 1);
        *(this->begin() + position) = std::move(value);
        ++vector_size;
        return this->begin() + position;
    }

    void PopBack() noexcept {
        assert(vector_size != 0);
        --vector_size;
    }

    Iterator Erase(ConstIterator pos) {
        Type* pos_temp = const_cast<Type*>(pos);
        std::move((pos_temp + 1), this->end(), pos_temp);
        --vector_size;
        return pos_temp;
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(this->ptr, other.ptr);
        std::swap(this->vector_size, other.vector_size);
        std::swap(this->capacity, other.capacity);
    }

    ~SimpleVector() = default;

    size_t GetSize() const noexcept {
        return vector_size;
    }

    size_t GetCapacity() const noexcept {
        return capacity;
    }

    bool IsEmpty() const noexcept {
        return vector_size == 0;
    }

    Type& operator[](size_t index) noexcept
    {
        return *(ptr.Get() + index);
    }

    const Type& operator[](size_t index) const noexcept {
        return *(ptr.Get() + index);
    }

    Type& At(size_t index) {
        if (index >= vector_size) {
            throw std::out_of_range("out_of_range");
        }
        return *(ptr.Get() + index);
    }

    const Type& At(size_t index) const {
        if (index >= vector_size) {
            throw std::out_of_range("out_of_range");
        }
        return *(ptr.Get() + index);
    }

    void Clear() noexcept {
        vector_size = 0;
    }

    void Resize(size_t new_size) {
        if (new_size <= vector_size) {
            vector_size = new_size;
        }
        else if (new_size <= capacity) {
            for (size_t i = vector_size; i < new_size; ++i)
            {
                new(ptr.Get() + i) Type(); 
            }
            vector_size = new_size;
        }
        else {
            this->Reserve(new_size);
            for (size_t i = vector_size; i < new_size; ++i) {
                new(ptr.Get() + i) Type();  
            }
            vector_size = new_size;
        }
    }

    Iterator begin() noexcept {
        return ptr.Get();
    }

    Iterator end() noexcept {
        return ptr.Get() + vector_size;
    }

    ConstIterator begin() const noexcept {
        return ptr.Get();
    }

    ConstIterator end() const noexcept {
        return ptr.Get() + vector_size;
    }

    ConstIterator cbegin() const noexcept {
        return ptr.Get();
    }

    ConstIterator cend() const noexcept {
        return ptr.Get() + vector_size;
    }

private:
    ArrayPtr<Type> ptr;
    size_t vector_size = 0;
    size_t capacity = 0;
};


template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs); 
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs; 
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
