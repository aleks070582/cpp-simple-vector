#pragma once
#include <cassert>
#include <cstdlib>

template <typename Type>
class ArrayPtr {
public:
  
    ArrayPtr() = default;
    
    explicit ArrayPtr(size_t size)
    {
        if (size == 0)
        {
            raw_ptr_ = nullptr;
        }
        else
        {
            raw_ptr_ = new Type[size];
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept
        : raw_ptr_(raw_ptr)
    {}

    ArrayPtr(ArrayPtr&& other) noexcept
        : raw_ptr_(other.raw_ptr_)
    {
        other.raw_ptr_ = nullptr;
    }

    ArrayPtr(const ArrayPtr&) = delete;

    ~ArrayPtr()
    {
        delete[] raw_ptr_;
    }

    ArrayPtr& operator=(ArrayPtr&& other) noexcept
    {
        if (this != &other)
        {
            delete[] raw_ptr_;  
            raw_ptr_ = other.raw_ptr_;
            other.raw_ptr_ = nullptr;
        }
        return *this;
    }

    ArrayPtr& operator=(const ArrayPtr&) = delete;

    [[nodiscard]] Type* Release() noexcept
    {
        Type* temp = raw_ptr_;
        raw_ptr_ = nullptr;
        return temp;
    }

    Type& operator[](size_t index) noexcept
    {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept
    {
        return raw_ptr_[index];
    }

    explicit operator bool() const
    {
        return raw_ptr_ != nullptr;
    }

    Type* Get() const noexcept
    {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept
    {
        using std::swap;
        swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};
