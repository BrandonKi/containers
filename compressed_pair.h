#ifndef COMPRESSED_PAIR_H
#define COMPRESSED_PAIR_H

template <typename Base, typename T>
class compressed_pair : Base {
  public:
    compressed_pair(T second_element_p): second_element{second_element_p} {

    }

    Base& first() {
        return *this;
    }

    T& second() {
        return second_element;
    }

  private:
    T second_element;
};

#endif //COMPRESSED_PAIR_H
