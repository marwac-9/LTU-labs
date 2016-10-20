#ifndef COP4530_VECTOR_H
#define COP4530_VECTOR_H
/********************************
 * COP 4530
 * FSU Department of Computer Sience
 ********************************/

#include <iostream>
#include <stdexcept>

namespace cop4530 {

template <typename T>
class Vector {
 public:
    // iterator support
    typedef T* iterator;
    typedef const T* const_iterator;

    // constructor, destructor, and copy constructor
    Vector(); // default zero parameter constructor
    Vector(const Vector &rhs); // copy constructor
    // num elements with value of val
    explicit Vector(int num, const T& val = T());
    // constructs with elements [start, end)
    Vector(const_iterator start, int num);

    ~Vector(); // destructor

    // operators

    // index operator
    T& operator[](int index);
    const T& operator[](int index) const;

    // assignment operator
    const Vector& operator=(const Vector &rhs);

    // at function. safer than operator[]
    T& at(int loc );
    const T& at(int loc ) const;

    T &front(); // reference to the first element
    const T& front() const;
    T &back(); // reference to the last element
    const T & back() const;

    // accessor member functions
    int size() const; // number of elements
    int capacity() const; // capacity of vector
    bool empty() const; // check if list is empty

    // mutator member functions
    void clear(); // delete all elements

    void push_back(const T & val); // insert to the end
    T pop_back(); // delete and return last element
    void push_front(const T & val); // insert to the end
    T pop_front(); // delete and return first element


    void resize(int num); // resize vector
    void reserve(int size); // reserve memory

    // print out all elements. ofc is deliminitor
    void print(std::ostream& os, char ofc = ' ') const;

    // iterator support
    iterator begin(); // iterator to first element
    const_iterator begin() const;
    iterator end(); // end marker iterator
    const_iterator end() const;
    iterator insert(iterator itr, const T& val); // insert val ahead of itr
    iterator erase(iterator itr); // erase one element
    iterator erase(iterator start, iterator end); // erase [start, end)


private:
    int theSize; // number of elements
    int theCapacity; // number of elements that can be stored
    T *array; // dynamic array to store elements.

    void doubleCapacity();
}; // end of class Vector

// overloading comparison operators
template <typename T>
bool operator==(const Vector<T> & lhs, const Vector<T> &rhs);

template <typename T>
bool operator!=(const Vector<T> & lhs, const Vector<T> &rhs);

// overloading output operator
template <typename T>
std::ostream& operator<<(std::ostream &os, const Vector<T> &v);

// include the implementation file here
#include "Vector.hpp"

} // end of namespace COP4530
#endif
