#ifndef ALIGNEDMEMORYBLOCK_H
#define ALIGNEDMEMORYBLOCK_H

#include <memory>

#if defined(WIN32)
# define aligned_alloc(a, sz)   _aligned_malloc(sz, a)
# define aligned_free(p)        _aligned_free(p)
#else
//# define aligned_alloc(a, sz) // is present at clib
# define aligned_free(p)        free(p)
#endif

namespace corecvs {

struct AlignedMemoryBlock
{
    AlignedMemoryBlock(size_t size, size_t align)
    {
        align = std::max(align, (size_t)1);
        CORE_ASSERT_TRUE_S(CORE_IS_POW2N(align));
        mData = aligned_alloc(align, ((size + align - 1) / align) * align);
    }

    AlignedMemoryBlock() : mData(nullptr)
    {}

    ~AlignedMemoryBlock()
    {
        aligned_free(mData);
        mData = nullptr;
    }

    void* getAlignedStart()
    {
        return mData;
    }

private:
    //AlignedMemoryBlock(const AlignedMemoryBlock&);

    void* mData;
};


struct AMBReference
{
    template<typename D>
    AMBReference(size_t size, size_t align, const D &deleter)
        : mSharedPtrBlock(new AlignedMemoryBlock(size, align), deleter)
    {}

    AMBReference(size_t size, size_t align) : mSharedPtrBlock(new AlignedMemoryBlock(size, align))
    {}

    AMBReference() : mSharedPtrBlock(nullptr)
    {}

    void* getAlignedStart()
    {
        return mSharedPtrBlock->getAlignedStart();
    }

    static size_t getTotalObjectSize(size_t size, size_t alignMask = 0)
    {
        return size + 2 * alignMask;
    }

private:
    std::shared_ptr<AlignedMemoryBlock> mSharedPtrBlock;
};

} // namespace corecvs

#endif // ALIGNEDMEMORYBLOCK_H
