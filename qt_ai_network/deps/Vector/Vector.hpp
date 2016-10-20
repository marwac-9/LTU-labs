/*
 * File:   Vector.hpp
 * Author: Abhishek Deshpande
 *
 * Created on January 30, 2011, 12:21 PM
 */

#ifndef VECTOR_HPP
#define  VECTOR_HPP
#include "Vector.h"
using namespace std;

/******************************************************************************/

/******************************Constructors***********************************/
template <typename T>
Vector<T>::Vector() {
    theSize = 0;
    theCapacity = 0;
    array = NULL;
}

template <typename T>
Vector<T>::Vector(const Vector & rhs) {
    operator=(rhs);
}

template<typename T>
Vector<T>::Vector(int num, const T& val) {
    theSize = num;
    theCapacity = num;
    array = new T[theCapacity];
    for (int i = 0; i < theSize; i++)
        array[i] = val;
}

template <typename T>
Vector<T>::Vector(const_iterator start, int num) {
    theSize = num;
    theCapacity = num;
    array = new T[theCapacity];
    for (int i = 0; i < theSize; i++) {
        array[i] = *start;
        start++;
    }
}

template <typename T>
Vector<T>::~Vector() {

    delete [] array;
    array = 0;
}

template <typename T>
T& Vector<T>::operator[](int index) {
    return array[index];
}

template <typename T>
const T& Vector<T>::operator[](int index) const {
    return array[index];
}
/******************************************************************************/

/**********************Positional Operations**********************************/
template <typename T>
const T& Vector<T>::at(int loc)const {
    if (loc > theSize) {
        throw out_of_range("Out of range value\n");
    } else
        return array[loc];
}

template <typename T>
T& Vector<T>::at(int loc) {
    if (loc > theSize) {
        throw out_of_range("Out of range\n");
    } else
        return array[loc];
}

template <typename T >
T & Vector<T>::front() {
    return array[0];
}

template <typename T>
const T & Vector<T>::front()const {
    return array[0];
}

template <typename T >
T & Vector<T>::back() {
    return array[theSize - 1];
}

template <typename T>
const T & Vector<T>::back() const {
    return array[theSize - 1];
}

template <typename T>
int Vector<T>::size() const {
    return theSize;
}

template <typename T>
int Vector<T>::capacity() const {

    return theCapacity + 1;
}

template <typename T>
bool Vector<T>::empty() const {

    if (size() == 0)
        return true;
    else
        return false;
}

template <typename T>
void Vector<T>::clear() {
    delete [] array;
    theSize = 0;
}

template <typename T>
void Vector<T>::push_back(const T & val) {
    if (theSize == theCapacity)

        reserve(2 * theCapacity + 1);

    array[theSize++] = val;
}

template <typename T>
T Vector<T>::pop_back() {
    theSize--;
    return *array;
}

template <typename T>
void Vector<T>::push_front(const T & val) {
    if (theSize == theCapacity)
        reserve(size());
    for (int i = theSize; i > 0; i--)
        array[i + 1] = array[i];

    array[0] = val;
}

template <typename T>
T Vector<T>::pop_front() {
    for (int i = 0; i < theSize; i++) {
        array[i] = array[i + 1];
    }
    theSize--;
    return *array;
}

template <typename T>
void Vector<T>::resize(int newSize) {
    if (newSize > theCapacity)
        reserve(newSize * 2 + 1);
    else
        theSize = newSize;
}

template <typename T>
void Vector<T>::reserve(int newCapacity) {
    if (newCapacity < theSize)
        return;

    T *oldArray = array;

    array = new T[ newCapacity ];
    for (int k = 0; k < theSize; k++)
        array[k] = oldArray[k];

    theCapacity = newCapacity;

    delete [] oldArray;
}
/******************************************************************************/

/********************************Iterator Methods*****************************/
template <typename T>
typename Vector<T>::iterator Vector<T>::begin() {
    return &array[0];
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::begin() const {
    return &array[0];
}

template <typename T>
typename Vector<T>::iterator Vector<T>::end() {
    return &array[theSize];
}

template <typename T>
typename Vector<T>::const_iterator Vector<T>::end() const {
    return &array[theSize];
}

template <typename T>
typename Vector<T>::iterator Vector<T>::erase(iterator itr) {

    while (itr != end()) {
        *itr = ++*itr;
        ++itr;
    }
    theSize--;
    return itr;
}

template <typename T>
typename Vector<T>::iterator Vector<T>::erase(iterator begin, iterator end) {

    begin = &array[0];
    end = &array[theSize];

    *begin = *(end - 1);
    theSize = 1;
    return begin;
}

template <typename T >
typename Vector<T>::iterator Vector<T>::insert(iterator itr, const T & val) {
    iterator itr1;
    if (theSize == theCapacity || theSize == 0)
        reserve(size());

    for (itr1 = end(); itr1 > itr; --itr1) {
        *itr1 = --*itr1;
    }
    *itr = val;
}
/****************************************************************************/

/************************Non class global functions**************************/
template <typename T>
        const Vector<T> & Vector<T>::operator=(const Vector<T> &rhs) {
    if (this != &rhs) {
        delete [] array;
        theSize = rhs.size();
        theCapacity = rhs.theCapacity;
        array = new T[ capacity()];
        for (int i = 0; i < size(); i++)
            array[i] = rhs.array[i];
    }
    return *this;
}

template <typename T>
bool operator ==(const Vector<T> &lhs, const Vector<T> &rhs) {
    int flag_chk = 0;
    if (lhs.size() == rhs.size()) {
        for (int i = 0; i < lhs.size(); i++) {
            if (lhs[i] != rhs[i]) {
                flag_chk = 1;
            }
        }
        if (flag_chk == 1) return false;
        else return true;
    } else
        return false;

}

template <typename T>
bool operator!=(const Vector<T> &lhs, const Vector<T> &rhs) {
    int flag_chk = 0;
    if (lhs.size() != rhs.size()) {
        for (int i = 0; i < lhs.size(); i++) {
            if (lhs[i] == rhs[i]) {
                flag_chk = 1;
            }
        }
        if (flag_chk == 1) return false;
        else return true;
    } else
        return false;
}

/******************************************************************************/
/*Print functions and overloaded operators*/
template <typename T>
void Vector<T>::print(std::ostream& os, char ofc) const {
    for (int i = 0; i < theSize; i++) {
        cout << array[i];
        os << ofc;
    }
}

template <typename T>
ostream & operator<<(std::ostream &os, const Vector<T> &v) {
    v.print(os, ' ');
    return os;
}

/*Capacity*/
template <typename T>
void Vector<T>::doubleCapacity() {
    if (theCapacity == 0)
        theCapacity = 1;
    else
        2 * theCapacity + 1;
}




#endif	/* VECTOR_HPP */

