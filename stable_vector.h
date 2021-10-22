#ifndef STABLE_VECTOR_H
#define STABLE_VECTOR_H

#include <memory>


template <typename T, size_t BucketSize, typename Allocator>
struct stable_vector_bucket {

    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<T>;
    using traits = std::allocator_traits<Alloc>;

    stable_vector_bucket(): next_bucket{nullptr}, index{0}, data{}, alloc{} {
        data = traits::allocate(alloc, BucketSize);
    }

    inline bool push(T t) {
        if(index + 1 <= BucketSize) {
            traits::construct(alloc, data + index, t);
            ++index;
            return false;
        }
        else {
            return true;
        }
    }

    T& at(std::size_t i) {
        return data[i];
    }

    const T& at(std::size_t i) const {
        return data[i];
    }

    stable_vector_bucket<T, BucketSize, Allocator>* next_bucket;
    size_t index;
    // put these two in a compressed pair
    T* data;
    Alloc alloc;
};

template <typename T, size_t BucketSize = 32, typename Allocator = std::allocator<T>>
class stable_vector {

    using Alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<stable_vector_bucket<T, BucketSize, Allocator>>;
    using traits = std::allocator_traits<Alloc>;

  public:
    stable_vector(): num_buckets{1}, alloc{} {
        first_bucket = traits::allocate(alloc, num_buckets);
        traits::construct(alloc, first_bucket);
        last_bucket = first_bucket;
    }

    void push_back(T t) {
        bool reallocate = last_bucket->push(t);
        if(!reallocate)
            return;
        allocate_bucket();
        // there's a possibility for some weird stuff to happen here...
        // probably should static assert bucket size isn't 0
        last_bucket->push(t);
    }

    size_t size() {
        return ((num_buckets - 1) * BucketSize) + last_bucket->index;
    }

    bool empty() {
        return first_bucket->index == 0;
    }

    // is this too slow?
    // if BucketSize is base 2 then "&" can be used instead of "%"
    // also maybe store an array of bucket pointers instead of traversing a linked list
    T& at(size_t index) {
        stable_vector_bucket<T, BucketSize, Allocator>* current_bucket = first_bucket;
        for(size_t i = 0; i < (index / BucketSize); ++i)
            current_bucket = current_bucket->next_bucket;
        return current_bucket->at(index % BucketSize);
    }

    T& operator[](size_t index) {
        return at(index);
    }

    const T& operator[](size_t index) const {
        return at(index);
    }

  private:

    void allocate_bucket() {
        auto* new_bucket = traits::allocate(alloc, 1);
        traits::construct(alloc, new_bucket);
        last_bucket->next_bucket = new_bucket;
        last_bucket = new_bucket;
    }

    // put these two into a "compressed pair" to save space
    size_t num_buckets;
    Alloc alloc;

    stable_vector_bucket<T, BucketSize, Allocator>* first_bucket;
    stable_vector_bucket<T, BucketSize, Allocator>* last_bucket;

};

#endif //STABLE_VECTOR_H
