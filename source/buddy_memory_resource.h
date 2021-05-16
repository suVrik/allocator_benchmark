#pragma once

#include "memory_resource.h"

class BuddyMemoryResource : public MemoryResource {
public:
    BuddyMemoryResource(MemoryResource* memory_resource, size_t root_size_log2, size_t leaf_size_log2);
    ~BuddyMemoryResource() override;

    void* allocate(size_t size, size_t alignment) override;
    void deallocate(void* memory) override;

private:
    static constexpr uint32_t END = 0x07FFFFFF;
    static constexpr uint32_t BUSY = 0x07FFFFFE;

    struct Leaf {
        uint32_t next : 27;
        uint32_t depth : 5;
    };

    MemoryResource* m_memory_resource;

    uint32_t m_leaf_size_log2;
    uint32_t m_max_depth;

    uint32_t* m_heads;
    Leaf* m_leafs;
    void* m_memory;
};
