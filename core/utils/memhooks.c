#include <stdio.h>
#include <stdlib.h>

#include "global.h"


#ifdef PROFILE_MEMORY

int memcount = 0;

typedef struct memblock_TAG
{
    void *ptr;
    size_t memsize;
} memblock;

#define BLOCKS 1000

memblock mem_blocks[BLOCKS];
int blocks_count = 0;

void *malloc_hook (size_t size)
{
    if (blocks_count >= BLOCKS)
    {
        printf("To many blocks allocated to profile\n");
    }
    if (size == 0)
    {
        printf("Attempting to malloc allocate 0 bytes\n");
    }
    void *ptr = malloc(size);

    CORE_ASSERT_FALSE((ptr == NULL), "Out of heap memory\n");
    memcount += size;
    mem_blocks[blocks_count].ptr = ptr;
    mem_blocks[blocks_count].memsize = size;
    blocks_count++;

    printf("Allocated %d memory, totaly %d, BK=%d\n", size, memcount, blocks_count);
    return ptr;
}

void *calloc_hook (size_t num, size_t size)
{
    if (blocks_count >= BLOCKS)
    {
        printf("To many blocks allocated to profile\n");
    }
    if (size == 0)
    {
        printf("Attempting to calloc allocate 0 bytes\n");
    }
    void *ptr = calloc(num, size);
    if (ptr == NULL)
    {
        CORE_ASSERT_FAIL("Out of heap memory");
    }
    memcount += size * num;
    mem_blocks[blocks_count].ptr = ptr;
    mem_blocks[blocks_count].memsize = size * num;
    blocks_count++;

    printf("Allocated %d memory, totaly %d, BK=%d\n", size * num, memcount, blocks_count);
    return ptr;
}

void free_hook (void *ptr)
{
    if (ptr == NULL)
    {
        CORE_ASSERT_FAIL("Attempt to free NULL pointer");
    }
    int i;
    for (i = 0; i < blocks_count; i++)
    {
        if (ptr == mem_blocks[i].ptr)
        {
            free(ptr);
            memcount -= mem_blocks[i].memsize;
            printf("Free %d memory, totaly used %d\n", mem_blocks[i].memsize, memcount);

            int j;
            for (j = i + 1; j < blocks_count; j++)
            {
                mem_blocks[j - 1] = mem_blocks[j];
            }

            blocks_count--;
            return;
        }
    }

    CORE_ASSERT_FAIL("Attempt to free unallocated pointer\n");
}

#endif
