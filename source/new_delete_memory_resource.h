#pragma once

#include "memory_resource.h"

class NewDeleteMemoryResource : public MemoryResource {
public:
    void* allocate(size_t size, size_t alignment) override;
    void deallocate(void* memory) override;
};
