#pragma once

#include <cstdint>

class MemoryResource {
public:
    virtual ~MemoryResource() = default;

    virtual void* allocate(size_t size, size_t alignment) = 0;
    virtual void deallocate(void* memory) = 0;

    template <typename T>
    T* allocate(size_t count) {
        return static_cast<T*>(allocate(sizeof(T) * count, alignof(T)));
    }
};
