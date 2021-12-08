#pragma once

template <class T>
struct node_
{
	T data_{};
	int priority_{};
	struct node_* next_{};
};

template <class T>
class priority_queue
{
public:
	priority_queue() = default;
	~priority_queue();

	const T& front() const;
	T& front();

	const T& back() const;
	T& back();

	const T& top() const;
	T& top();

	void push(const T& value, const int& priority);
	void pop();

	bool empty() const;

	int size() const;
private:
	node_<T>* first_ = {};
	node_<T>* last_ = {};
};
#include "priority_queue.inl"