#pragma once

#include "memory_resource.h"

class PagedPoolMemoryResource final : public MemoryResource {
public:
    PagedPoolMemoryResource(MemoryResource* memory_resource, size_t allocation_size, size_t allocations_per_page);
    ~PagedPoolMemoryResource() override;

    void* allocate(size_t size, size_t alignment) override;
    void deallocate(void* memory) override;

private:
    void allocate_new_page(void* previous_page);

    MemoryResource* m_memory_resource;

    size_t m_allocation_size;
    size_t m_allocations_per_page;

    void* m_page_head;
    void* m_data_head;
};
