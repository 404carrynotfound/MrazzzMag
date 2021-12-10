 #pragma once

 constexpr int MIN_SIZE = { 8 };

 template <class T>
 class Queue
 {
 public:
 	Queue() = default;
 	~Queue();

 	Queue(const Queue<T>& other_queue);
 	Queue<T>& operator=(const Queue<T>& other_queue);

 	const T& front() const;
 	T& front();

 	const T& back() const;
 	T& back();

 	void push(const T& value);
 	void pop();

 	bool empty() const;

 	int size() const;

 private:
 	void clear();
 	void resize();

 	T* storage_ = { new T[MIN_SIZE] };
 	int capacity_ = { MIN_SIZE };
 	int first_ = {};
 	int last_ = {};
 };

 #include "queue.inl"