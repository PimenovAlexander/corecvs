#pragma once
/**
 * \file memoryBlock.h
 * \brief Add Comment Here
 *
 * \date Apr 9, 2011
 * \author alexander
 */

#include <new>
#include <stdint.h>
#include <stdlib.h>

#include "core/utils/global.h"

#include "core/utils/atomicOps.h"

namespace corecvs {


/**
 *  This class is a class that holds a memory block with it's reference count
 *
 **/
template<typename Type>
class ObjectBlock
{
public:
    uint16_t   flags;        /**< Flags of the block*/
private:
    atomic_int refcount;     /**< Reference count for the block */
public:
    Type 	   data;         /**< Data start */

    /**
     * Block constructor
     * As this reference will be returned somewhere refcount is set to 1.
     * TODO: Check if it is good idea to set refcount here.
     *
     **/
    ObjectBlock()
        : flags(0)
    {
        refcount.store(0);
    }

    atomic_int &refCount()
    {
        return refcount;
    }
};

/**
 *  This class is an smart pointer to the memory block.
 *  It counts references to the block (counter is stored in a block itself)
 *
 *  This class can be used for two purposes.
 *    1. Allocate a piece of memory with automatic reference count
 *    2. Allocate an object with automatic reference count
 *
 **/
template<typename Type>
class ObjectRef
{
public:
	ObjectBlock<Type> *block;

    ObjectRef() : block(NULL)
    {
        DOTRACE(("Calling default constructor for ObjectRef<> at %p. Block at %p\n", (void *)this, (void *)block));
    }


    explicit ObjectRef(bool shouldAllocate) : block(NULL)
    {
        DOTRACE(("Calling specific constructor for ObjectRef<> at %p. Block at %p\n", (void *)this, (void *)block ));
        if (shouldAllocate)
        {
            allocate();
        }
    }

    size_t getTotalObjectSize(size_t size = sizeof(Type), size_t alignMask = 0) const
    {
        return size + 2 * alignMask + sizeof(ObjectBlock<Type>) - sizeof(Type);
    }

    /**
     *   In the worst case we will have to add align mask twice. Say we want
     * to allocate 10 bytes with 4 byte alignment.
     *
     *
     *  |---X XXXX XXXX X---|
     *
     *
     **/
    ObjectBlock<Type> * allocate(size_t size = sizeof(Type), size_t alignMask = 0)
    {
        if (this->block != NULL)
        {
            freeRef();
        }

        size_t totalSize = getTotalObjectSize(size, alignMask);
        void *rawBlock = new uint8_t[totalSize];
        if (rawBlock != NULL)
        {
            this->block = new(rawBlock) ObjectBlock<Type>();
            addRef();
        } /* Otherwise NULL will remain and we will return it */

        DOTRACE(("Calling allocate for MemoryBlockRef:\n"
        		 "   Position is %p\n"
                 "   Raw position %p\n"
        		 "   Size is %" PRISIZE_T "\n"
        		 "   Allocated size is %" PRISIZE_T "\n"
        		 "   Data start is %p\n"
        		 "   Align start is %p\n",
                this->block,
                (void *)rawBlock,
                size,
                totalSize,
                getBlockStart(),
                getAlignedStart(alignMask)));

        return this->block;
    }

    void *getBlockStart()
    {
        return (void *)&(block->data);
    }

    Type *get()
    {
        return &(block->data);
    }


    void *getAlignedStart(size_t alignMask)
    {
        uintptr_t unalignedBlock = (uintptr_t)getBlockStart();
        uintptr_t   alignedBlock = (((unalignedBlock - 1) | alignMask) + 1);
        return (void *) alignedBlock;
    }


    int addRef()
    {
        if (this->block == NULL)
        {
            return 0;
        }
        atomic_inc_and_fetch(&(block->refCount()));
        DOTRACE(("Reference for block %p Increment to %d\n", (void *)block, block->refCount()));
        return block->refCount();
    }

    int freeRef()
    {
        if (this->block == NULL)
        {
            return 0;
        }
        CORE_ASSERT_FALSE_P(block->refCount() == 0, ("Block referenced by %p overfreed\n", (void *)this));

        DOTRACE(("Reference for block %p Decrement to %d\n", (void *)block, block->refCount() - 1));
        int new_count = atomic_dec_and_fetch(&(block->refCount()));
        if (new_count == 0)
        {
            block->~ObjectBlock<Type>();

            DOTRACE(("Deleting block %p\n", (void *)this->block));
            uint8_t *rawBlock = (uint8_t *)(void *)(this->block);
            delete[] rawBlock;
            this->block = NULL;
            return 0;
        }
        return block->refCount();
    }

    ObjectRef &operator =(const ObjectRef &other)
    {
    	DOTRACE(("Assigning reference with block %p to reference with block %p\n", (void *)other.block, (void *)block));
        if (this->block != NULL)
        {
            freeRef();
        }
        this->block = other.block;
        addRef();
        return *this;
    }

    ObjectRef (const ObjectRef &other) : block( NULL )
    {
        operator =(other);
    }


    ~ObjectRef()
    {
        freeRef();
    }
};

typedef ObjectBlock<uint8_t> MemoryBlock;
typedef ObjectRef<uint8_t> MemoryBlockRef;




/*
template<typename Type>
class SharedObject : public MemoryBlockRef {
	MemoryBlockRef memoryBlock;
	SharedObject()
	{
		object = memoryBlock.allocate(sizeof(Type));
	}

	Type *getData()
	{
		return (Type *) memoryBlock.getBlockStart();
	}
};
*/

/**
 *  Temporary solution for scoped pointer
 **/
template<typename Type>
class ScopedRef
{
public:
    Type *pointer;

    ScopedRef(Type *_pointer = NULL) :
        pointer(_pointer)
    {
    }

    Type* operator->()
    {
        return pointer;
    }

    bool isNull() {
        return (pointer == NULL);
    }


    ~ScopedRef()
    {
        delete_safe(pointer);
    }
};

} //namespace corecvs
