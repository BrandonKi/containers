#ifndef BUCKET_LIST_H
#define BUCKET_LIST_H

#include <memory>

namespace bucket_list_internal {

template <typename T, size_t BucketSize, typename Allocator>
struct bucket {

    //using BucketAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<stable_vector_bucket>;
    //using BucketTraits = std::allocator_traits<Alloc>;
    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<T>;
    using Traits = std::allocator_traits<Alloc>;

    bucket(): next_bucket{nullptr}, index{0}, data{} {

    }

    inline void internal_allocate(Alloc& alloc) {
        data = Traits::allocate(alloc, BucketSize);
    }

    inline void internal_deallocate(Alloc& alloc) {
        Traits::destroy(alloc, data);
        Traits::deallocate(alloc, data, BucketSize);
    }

    inline bool push(Alloc& alloc, T t) {
        if(index + 1 <= BucketSize) {
            Traits::construct(alloc, data + index, t);
            ++index;
            return false;
        }
        else {
            return true;
        }
    }

    inline T& at(size_t i) {
        return data[i];
    }

    inline const T& at(size_t i) const {
        return data[i];
    }

    bucket<T, BucketSize, Allocator>* next_bucket;
    // each bucket doesn't need to track its index
    size_t index;
    T* data;
};

}

template <typename T, size_t BucketSize = 32, typename Allocator = std::allocator<T>>
class bucket_list {

    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<bucket_list_internal::bucket<T, BucketSize, Allocator>>;
    using Traits = std::allocator_traits<Alloc>;
    using SubAlloc = typename std::allocator_traits<Allocator>::template rebind_alloc<T>;
    using SubTraits = std::allocator_traits<SubAlloc>;

  public:
    bucket_list(): num_buckets{1}, alloc{}, sub_alloc{} {
        static_assert(BucketSize > 0);
        first_bucket = Traits::allocate(alloc, num_buckets);
        Traits::construct(alloc, first_bucket);
        first_bucket->internal_allocate(sub_alloc);
        last_bucket = first_bucket;
    }

    ~bucket_list() {
        while(first_bucket) {
            auto* temp = first_bucket->next_bucket;
            first_bucket->internal_deallocate(sub_alloc);
            Traits::destroy(alloc, first_bucket);
            Traits::deallocate(alloc, first_bucket, 1);
            first_bucket = temp;
        }
    }

    void push_back(T t) {
        bool reallocate = last_bucket->push(sub_alloc, t);
        if(!reallocate)
            return;
        allocate_bucket();
        last_bucket->push(sub_alloc, t);
    }

    size_t size() {
        return ((num_buckets - 1) * BucketSize) + last_bucket->index;
    }

    bool empty() {
        return first_bucket->index == 0;
    }

    // is this too slow?
    // maybe store an array of bucket pointers instead of traversing a linked list
    T& at(size_t index) {
        bucket_list_internal::bucket<T, BucketSize, Allocator>* current_bucket = first_bucket;

        if constexpr (BucketSize % 2 == 0) {
            for(size_t i = 0; i < (index / BucketSize); ++i)
                current_bucket = current_bucket->next_bucket;
            return current_bucket->at(index & (BucketSize - 1));
        }
        else {
            for(size_t i = 0; i < (index / BucketSize); ++i)
                current_bucket = current_bucket->next_bucket;
            return current_bucket->at(index % BucketSize);
        }
    }

    T& operator[](size_t index) {
        return at(index);
    }

    const T& operator[](size_t index) const {
        return at(index);
    }

  private:

    void allocate_bucket() {
        auto* new_bucket = Traits::allocate(alloc, 1);
        Traits::construct(alloc, new_bucket);
        new_bucket->internal_allocate(sub_alloc);
        last_bucket->next_bucket = new_bucket;
        last_bucket = new_bucket;
    }

    // 8
    size_t num_buckets;

    // put all of these into a "compressed pair" to save space
    // 8
    bucket_list_internal::bucket<T, BucketSize, Allocator>* first_bucket;
    // 8
    bucket_list_internal::bucket<T, BucketSize, Allocator>* last_bucket;
    Alloc alloc;
    SubAlloc sub_alloc;
};

#endif //BUCKET_LIST_H
