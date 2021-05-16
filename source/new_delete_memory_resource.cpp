#include "new_delete_memory_resource.h"

#include <malloc.h>

void* NewDeleteMemoryResource::allocate(size_t size, size_t alignment) {
    return _aligned_malloc(size, alignment);
}

void NewDeleteMemoryResource::deallocate(void* memory) {
    _aligned_free(memory);
}
