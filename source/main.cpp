#include "buddy_memory_resource.h"
#include "new_delete_memory_resource.h"
#include "paged_pool_memory_resource.h"

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>

struct AllocationTest {
    size_t size_log2;
    size_t count_log2;
};

static const AllocationTest TESTS[] = {
    { 3,  14 },
    { 4,  13 },
    { 6,  12 },
    { 8,  11 },
    { 10, 11 },
    { 12, 10 },
    { 14, 10 },
    { 16, 9  },
    { 18, 9  },
    { 20, 8  },
    { 22, 8  },
};

static void* pointers[20000];

static size_t rand64() {
    return static_cast<size_t>(rand() & UINT16_MAX) |
           (static_cast<size_t>(rand() & UINT16_MAX) << 16) |
           (static_cast<size_t>(rand() & UINT16_MAX) << 32) |
           (static_cast<size_t>(rand() & UINT16_MAX) << 48);
}

int main(int argc, char* argv[]) {
    for (const AllocationTest& test : TESTS) {
        size_t size = 1ull << test.size_log2;
        size_t count = 1ull << test.count_log2;

        NewDeleteMemoryResource new_delete_memory_resource;

        long long malloc_time;
        long long free_time;

        {
            auto t1 = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < count; i++) {
                pointers[i] = new_delete_memory_resource.allocate(size, 1);
            }

            auto t2 = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < count; i++) {
                new_delete_memory_resource.deallocate(pointers[i]);
            }

            auto t3 = std::chrono::high_resolution_clock::now();

            malloc_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
            free_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t2).count();

            std::cout << "NEW DELETE " << std::setw(10) << size << ":" << std::setw(10) << malloc_time << " / " << std::setw(10) << free_time << std::endl;
        }

        BuddyMemoryResource buddy_memory_resource(&new_delete_memory_resource, test.size_log2 + test.count_log2, test.size_log2);

        {
            auto t1 = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < count; i++) {
                pointers[i] = buddy_memory_resource.allocate(size, 1);
            }

            auto t2 = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < count; i++) {
                buddy_memory_resource.deallocate(pointers[i]);
            }

            auto t3 = std::chrono::high_resolution_clock::now();

            long long allocate_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
            long long deallocate_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t2).count();

            std::cout << "BUDDY      " << std::setw(10) << size << ":" << std::setw(10) << allocate_time << " / " << std::setw(10) << deallocate_time << " (" << static_cast<double>(malloc_time) / allocate_time << " / " << static_cast<double>(free_time) / deallocate_time << ")" << std::endl;
        }

        PagedPoolMemoryResource paged_pool_memory_resource(&new_delete_memory_resource, size, count / 4);

        {
            auto t1 = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < count; i++) {
                pointers[i] = paged_pool_memory_resource.allocate(size, 1);
            }

            auto t2 = std::chrono::high_resolution_clock::now();

            for (size_t i = 0; i < count; i++) {
                paged_pool_memory_resource.deallocate(pointers[i]);
            }

            auto t3 = std::chrono::high_resolution_clock::now();

            long long allocate_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
            long long deallocate_time = std::chrono::duration_cast<std::chrono::nanoseconds>(t3 - t2).count();

            std::cout << "PAGED POOL " << std::setw(10) << size << ":" << std::setw(10) << allocate_time << " / " << std::setw(10) << deallocate_time << " (" << static_cast<double>(malloc_time) / allocate_time << " / " << static_cast<double>(free_time) / deallocate_time << ")" << std::endl;
        }
    }

    return 0;
}
