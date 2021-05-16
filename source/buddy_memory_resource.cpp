#include "buddy_memory_resource.h"

#include <cassert>

BuddyMemoryResource::BuddyMemoryResource(MemoryResource* memory_resource, size_t root_size_log2, size_t leaf_size_log2)
    : m_memory_resource(memory_resource)
    , m_leaf_size_log2(static_cast<uint32_t>(leaf_size_log2))
    , m_max_depth(static_cast<uint32_t>(root_size_log2 - leaf_size_log2))
{
    assert(memory_resource != nullptr && "Invalid memory resource");
    assert(root_size_log2 >= leaf_size_log2 && "Root size must be not less than leaf size.");
    assert(root_size_log2 - leaf_size_log2 < 27 && "Binary tree height must be less than 27.");
    assert(leaf_size_log2 > 0 && "Leaf size must be greater than 0.");

    m_heads = memory_resource->allocate<uint32_t>(m_max_depth + 1ull);
    m_leafs = memory_resource->allocate<Leaf>(1ull << m_max_depth);
    m_memory = memory_resource->allocate<uint8_t>(1ull << root_size_log2);

    for (uint32_t i = 0; i < m_max_depth; i++) {
        m_heads[i] = END;
    }
    
    m_heads[m_max_depth] = 0;

    m_leafs[0].depth = m_max_depth;
    m_leafs[0].next = END;
}

BuddyMemoryResource::~BuddyMemoryResource() {
    assert(m_heads[m_max_depth] != END && "Not all memory is deallocated.");

    m_memory_resource->deallocate(m_memory);
    m_memory_resource->deallocate(m_leafs);
    m_memory_resource->deallocate(m_heads);
}

void* BuddyMemoryResource::allocate(size_t size, size_t alignment) {
    uint32_t depth = 0;
    size_t allocation_size = 1ull << m_leaf_size_log2;

    // Search for the smallest available node.
    while (depth < m_max_depth && (allocation_size < size || m_heads[depth] == END)) {
        depth++;
        allocation_size <<= 1;
    }

    // Check whether any node is found.
    if (depth == m_max_depth && (allocation_size < size || m_heads[m_max_depth] == END)) {
        return nullptr;
    }

    // Split this node into two smaller nodes recursively.
    uint32_t local_offset = m_heads[depth];
    assert(m_leafs[local_offset].next != BUSY);
    assert(m_leafs[local_offset].depth == depth);

    // Remove this node from this depth's linked list.
    m_heads[depth] = m_leafs[local_offset].next;

    // Split as many nodes as we can.
    while (depth > 0 && (allocation_size >> 1) >= size) {
        uint32_t buddy_offset = local_offset ^ (1u << (depth - 1));
        // Buddy has undefined next and depth.

        depth--;
        allocation_size >>= 1;

        // Insert buddy node into the next depth's linked list.
        uint32_t old_head = m_heads[depth];
        m_heads[depth] = buddy_offset;
        m_leafs[buddy_offset].next = old_head;
        m_leafs[buddy_offset].depth = depth;
    }

    // Mark the node as busy, so it's buddy node doesn't merge into a parent node. Depth could've changed too.
    m_leafs[local_offset].next = BUSY;
    m_leafs[local_offset].depth = depth;

    // Return absolute pointer.
    return static_cast<uint8_t*>(m_memory) + (static_cast<size_t>(local_offset) << m_leaf_size_log2);
}

void BuddyMemoryResource::deallocate(void* memory) {
    if (memory != nullptr) {
        assert(memory >= m_memory && memory < static_cast<uint8_t*>(m_memory) + (1ull << (m_leaf_size_log2 + m_max_depth)));

        size_t offset = static_cast<uint8_t*>(memory) - static_cast<uint8_t*>(m_memory);
        assert(((offset >> m_leaf_size_log2) << m_leaf_size_log2) == offset);

        uint32_t local_offset = static_cast<uint32_t>(offset >> m_leaf_size_log2);
        assert(m_leafs[local_offset].next == BUSY);

        uint32_t depth = m_leafs[local_offset].depth;
        assert(depth <= m_max_depth);

        uint32_t buddy_offset = local_offset ^ (1u << depth);

        // Merge as many nodes as we can.
        while (depth < m_max_depth && (m_leafs[buddy_offset].next != BUSY && m_leafs[buddy_offset].depth == depth)) {
            // Buddy offset must be stored in this depth's linked list.
            uint32_t current_offset = m_heads[depth];
            assert(current_offset != END);

            // Remove buddy offset from this depth's linked list.
            if (current_offset == buddy_offset) {
                // Buddy offset is stored in the head.
                m_heads[depth] = m_leafs[buddy_offset].next;
            } else {
                // Buddy offset is stored somewhere inside linked list.
                while (m_leafs[current_offset].next != buddy_offset) {
                    current_offset = m_leafs[current_offset].next;
                    assert(current_offset != END);
                }
                m_leafs[current_offset].next = m_leafs[buddy_offset].next;
            }

            depth++;

            // Calculate parent offset.
            local_offset = local_offset & buddy_offset;

            // Calculate parent buddy's offset.
            buddy_offset = local_offset ^ (1u << depth);
        }

        // Depth could've changed.
        m_leafs[local_offset].depth = depth;

        // Store offset in a linked list for other allocations.
        uint32_t old_head = m_heads[depth];
        m_heads[depth] = local_offset;
        m_leafs[local_offset].next = old_head;
    }
}
