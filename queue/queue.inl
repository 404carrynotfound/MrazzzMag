 #pragma once
 #include <cassert>

 #include "queue.h"

 template <class T>
 Queue<T>::~Queue()
 {
 	delete[] storage_;
 }

 template <class T>
 Queue<T>::Queue(const Queue<T>& other_queue)
 {
 	clear();
 	capacity_ = other_queue.capacity_;
 	storage_ = new T[capacity_];
 	for (int i = other_queue.first_, j = 0; i < other_queue.last_; ++i, ++j)
 	{
 		storage_[j] = other_queue.storage_[i];
 	}
 	last_ = other_queue.last_ - other_queue.first_ - 1;
 	first_ = 0;
 }

 template <class T>
 Queue<T>& Queue<T>::operator=(const Queue<T>& other_queue)
 {
 	if (this != &other_queue)
 	{
 		clear();
 		capacity_ = other_queue.capacity_;
 		storage_ = new T[capacity_];
 		for (int i = other_queue.first_, j = 0; i < other_queue.last_; ++i, ++j)
 		{
 			storage_[j] = other_queue.storage_[i];
 		}
 		first_ = 0;
 		last_ = other_queue.last_ - other_queue.first_ - 1;
 	}
 	return *this;
 }

 template <class T>
 const T& Queue<T>::front() const
 {
 	return storage_[first_];
 }

 template <class T>
 T& Queue<T>::front()
 {
 	return storage_[first_];
 }

 template <class T>
 const T& Queue<T>::back() const
 {
 	assert(last_ != 0);
 	return storage_[last_ - 1];
 }

 template <class T>
 T& Queue<T>::back()
 {
 	assert(last_ != 0);
 	return storage_[last_ - 1];
 }

 template <class T>
 void Queue<T>::push(const T& value)
 {
 	if (last_ == capacity_)
 	{
 		resize();
 	}
 	storage_[last_++] = value;
 }

 template <class T>
 void Queue<T>::pop()
 {
 	first_++;
 }

 template <class T>
 bool Queue<T>::empty() const
 {
 	return size() == 0;
 }

 template <class T>
 int Queue<T>::size() const
 {
 	return last_ - first_;
 }

 template <class T>
 void Queue<T>::clear()
 {
 	delete[] storage_;
 	capacity_ = { MIN_SIZE };
 	first_ = { 0 };
 	last_ = { 0 };
 }

 template <class T>
 void Queue<T>::resize()
 {
 	capacity_ *= 2;
 	T* temp = new T[capacity_];
 	for (int i = first_, j = 0; i < last_; ++i, ++j)
 	{
 		temp[j] = storage_[i];
 	}
 	last_ = last_ - first_;
 	first_ = 0;
 	delete[] storage_;
 	storage_ = temp;
 }