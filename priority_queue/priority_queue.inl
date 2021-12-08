#pragma once

#include <cassert>

#include "priority_queue.h"

template <class T>
priority_queue<T>::~priority_queue()
{
    auto current = first_;
    while (current != nullptr)
    {
        auto next = current->next_;
        delete current;
        current = next;
    }
}

template <class T>
const T& priority_queue<T>::front() const
{
	return first_->data_;
}

template <class T>
T& priority_queue<T>::front()
{
	return first_->data_;
}

template <class T>
const T& priority_queue<T>::back() const
{
	return last_->data_;
}

template <class T>
T& priority_queue<T>::back()
{
	return last_->data_;
}

template <class T>
const T& priority_queue<T>::top() const
{
	return first_->data_;
}

template <class T>
T& priority_queue<T>::top()
{
	return first_->data_;
}

template <class T>
void priority_queue<T>::push(const T& value, const int& priority)
{
	auto* const current = new node_<T>;
	current->data_ = value;
	current->priority_ = priority;

	if (!first_)
	{
		current->next_ = first_;
		first_ = current;
		last_ = current;
	}	
	else if (priority < first_->priority_)
	{
		current->next_ = first_;
		first_ = current;
	}
	else
	{
		node_<T>* temp = last_;

		if (!temp->next_)
		{
			current->next_ = temp->next_;
			temp->next_ = current;
			last_ = current;
			return;
		}

		while (temp->next_ && temp->next_->priority_ <= priority)
		{
			temp = temp->next_;
			current->next_ = temp->next_;
			temp->next_ = temp;
		}
	}
}

template <class T>
void priority_queue<T>::pop()
{
    assert(size() != 0);
    node_<T>* temp = first_;
	first_ = first_->next_;
	delete temp;
}

template <class T>
bool priority_queue<T>::empty() const
{
	return first_ == nullptr;
}

template <class T>
int priority_queue<T>::size() const
{
	int count = 0;
	auto temp = first_;
	while (temp)
	{
		count++;
		temp = temp->next_;
	}
	return count;
}