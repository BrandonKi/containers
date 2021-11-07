
#ifndef NOISY_ALLOCATOR_H
#define NOISY_ALLOCATOR_H

#include <stdlib.h>
#include <new>
#include <memory>
#include <iostream>
#include <limits>

template <typename T>
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
        std::cout << "Constructing noisy_allocator\n";
    }

    ~noisy_allocator() noexcept {
        std::cout << "Destructing noisy_allocator\n";
        std::cout << "allocated: " << num_allocated << ", deallocated: " << num_deallocated << "\n";
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
        num_allocated += (n * sizeof(T));
        std::cout << "allocating " << (n * sizeof(T)) << " bytes\n";
        return static_cast<pointer>(malloc(n * sizeof(T)));
    }

    [[nodiscard]] pointer allocate(size_type const n, const_void_pointer hint) const noexcept {
        pointer result = allocate(n);
        std::cout << "used hint: " << hint << "\n";
        return result;
    }

    void deallocate(pointer p, size_type const n) const noexcept {
        num_deallocated += (n * sizeof(T));
        std::cout << "deallocating " << (n * sizeof(T)) << " bytes\n";
        free(p);
    }

    template <typename T, typename... Args>
    void construct(pointer p, Args&&... args) {
        std::cout << "construct at: " << p << "\n";
        std::construct_at(p, std::forward<Args>(args)...);
    }

    template <typename T>
    void destroy(pointer p) {
        std::cout << "destroy at: " << p << "\n";
        std::destroy_at(p);
    }

    size_type max_size() noexcept {
        std::cout << "called max_size\n";
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    noisy_allocator select_on_container_copy_construction() {
        std::cout << "container copied\n";
        return noisy_allocator{};
    }

  private:
    inline static size_t num_allocated = 0;
    inline static size_t num_deallocated = 0;
};

#endif // NOISY_ALLOCATOR_H