/**
 Warning: Do not rely on scope logging granularity; it is not always correct.
**/


#ifndef NOISY_ALLOCATOR_H
#define NOISY_ALLOCATOR_H

#include <stdlib.h>
#include <new>
#include <memory>
#include <iostream>
#include <limits>

enum class log_granularity {
    allocation,
    scope,    // scope isn't perfect
    global,
    all,
};

namespace noisy_allocator_global_state {
    inline static size_t global_num_allocated = 0;
    inline static size_t global_num_deallocated = 0;
    inline static log_granularity granularity = log_granularity::all;
};


template <typename T = void>
class noisy_allocator {
  public:
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = std::pointer_traits<pointer>::template rebind<const value_type>;
    using void_pointer = std::pointer_traits<pointer>::template rebind<void>;
    using const_void_pointer = std::pointer_traits<pointer>::template rebind<const void>;;
    using difference_type = std::pointer_traits<pointer>::difference_type;
    using size_type = std::make_unsigned_t<difference_type>;
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::false_type;
    using propagate_on_container_swap = std::false_type;
    using is_always_equal = std::false_type;

    noisy_allocator() noexcept {
        using namespace noisy_allocator_global_state;
        using enum log_granularity;

        if(granularity == scope || granularity == all)
            std::cout << "Constructing noisy_allocator\n";
    }

    ~noisy_allocator() noexcept {
        using namespace noisy_allocator_global_state;
        using enum log_granularity;

        if(granularity == scope || granularity == all) {
            std::cout << "Destructing noisy_allocator\n";
            std::cout << "allocated: " << num_allocated << ", deallocated: " << num_deallocated << "\n";
        }
    }

    static void get_report() {
        using namespace noisy_allocator_global_state;

        std::cout << "\n";
        std::cout << "Allocated:   " << global_num_allocated << " bytes\n";
        std::cout << "Deallocated: " << global_num_deallocated << " bytes\n";
        std::cout << "Leaked:      " << (global_num_allocated - global_num_deallocated) << " bytes\n";
    }

    static void set_granularity(log_granularity lg) {
        using namespace noisy_allocator_global_state;
        using enum log_granularity;

        if(lg == scope)
            std::cout << "\nWarning: Do not rely on scope logging granularity it is not always correct.\n";

        granularity = lg;
    }

    // A converting copy constructor:
    template<typename U>
    noisy_allocator(const noisy_allocator<U>& other) noexcept {

    }

    template<typename U>
    bool operator==(const noisy_allocator<U>& other) const noexcept {
        return true;
    }

    template<typename U>
    bool operator!=(const noisy_allocator<U>&) const noexcept {
        return false;
    }

    [[nodiscard]] pointer allocate(size_type const n) const noexcept {
        using namespace noisy_allocator_global_state;
        using enum log_granularity;

        auto alloc_size = (n * sizeof(T));
        num_allocated += alloc_size;
        global_num_allocated += alloc_size;
        if(granularity == all || granularity == allocation)
            std::cout << "allocating " << alloc_size << " bytes\n";
        return static_cast<pointer>(malloc(alloc_size));
    }

    [[nodiscard]] pointer allocate(size_type const n, const_void_pointer hint) const noexcept {
        using namespace noisy_allocator_global_state;
        using enum log_granularity;

        pointer result = allocate(n);
        if(granularity == all)
            std::cout << "used hint: " << hint << "\n";
        return result;
    }

    void deallocate(pointer p, size_type const n) const noexcept {
        using namespace noisy_allocator_global_state;
        using enum log_granularity;

        auto dealloc_size = (n * sizeof(T));
        num_deallocated += dealloc_size;
        global_num_deallocated += dealloc_size;
        if(granularity == all || granularity == allocation)
            std::cout << "deallocating " << dealloc_size << " bytes\n";
        free(p);
    }

    template <typename T, typename... Args>
    void construct(pointer p, Args&&... args) {
        using namespace noisy_allocator_global_state;
        using enum log_granularity;

        if(granularity == all)
            std::cout << "construct at: " << p << "\n";
        std::construct_at(p, std::forward<Args>(args)...);
    }

    template <typename T>
    void destroy(pointer p) {
        using namespace noisy_allocator_global_state;
        using enum log_granularity;

        if(granularity == all)
            std::cout << "destroy at: " << p << "\n";
        std::destroy_at(p);
    }

    size_type max_size() noexcept {
        using namespace noisy_allocator_global_state;
        using enum log_granularity;

        if(granularity == all)
            std::cout << "called max_size\n";
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    noisy_allocator select_on_container_copy_construction() {
        using namespace noisy_allocator_global_state;
        using enum log_granularity;

        if(granularity == all)
            std::cout << "container copied\n";
        return noisy_allocator{};
    }

  private:
    inline static size_t num_allocated = 0;
    inline static size_t num_deallocated = 0;
};

#endif // NOISY_ALLOCATOR_H