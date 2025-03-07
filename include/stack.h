//
// Created by ctlf on 3/7/25.
//

#ifndef STACK_H
#define STACK_H

#define STACK_DEFAULT_COUNT 8
#define STACK_GROWTH_FACTOR 2

#include <stdexcept>
#include <memory>    // for std::uninitialized_copy

#define STACK_DEFAULT_COUNT 8
#define STACK_GROWTH_FACTOR 2

template<typename type_t>
class stack {
public:
    stack()
        : m_Data(static_cast<type_t*>(::operator new[](sizeof(type_t) * STACK_DEFAULT_COUNT))),
          m_Capacity(STACK_DEFAULT_COUNT), m_Count(0) {}

    ~stack() {
        clear();
        ::operator delete[](m_Data);
    }

    void push(const type_t& value) {
        if (m_Count == m_Capacity) {
            reallocate(m_Capacity * STACK_GROWTH_FACTOR);
        }
        new (&m_Data[m_Count]) type_t(value); // Placement new (construct object)
        ++m_Count;
    }

    void pop() {
        if (empty()) return;
        m_Data[--m_Count].~type_t(); // Manually call destructor
    }

    type_t& top() {
        if (empty()) throw std::runtime_error("Stack is empty");
        return m_Data[m_Count - 1];
    }

    bool empty() const {
        return m_Count == 0;
    }

    size_t size() const {
        return m_Count;
    }

    type_t* data() {
        return m_Data;
    }

    type_t* begin() {
        return m_Data;
    }

    type_t* end() {
        return m_Data + m_Count;
    }

    const type_t* cbegin() const {
        return m_Data;
    }
    const type_t* cend() const {
        return m_Data + m_Count;
    }

private:
    void reallocate(size_t newSize) {
        type_t* newData = static_cast<type_t*>(::operator new[](sizeof(type_t) * newSize));

        // Move old elements to new storage
        for (size_t i = 0; i < m_Count; i++) {
            new (&newData[i]) type_t(std::move(m_Data[i]));
            m_Data[i].~type_t();
        }

        ::operator delete[](m_Data); // Free old memory
        m_Data = newData;
        m_Capacity = newSize;
    }

    void clear() {
        for (size_t i = 0; i < m_Count; i++) {
            m_Data[i].~type_t(); // Call destructors
        }
        m_Count = 0;
    }

private:
    type_t* m_Data;
    size_t m_Capacity;
    size_t m_Count;
};


#undef STACK_DEFAULT_COUNT
#undef STACK_GROWTH_FACTOR

#endif //STACK_H
