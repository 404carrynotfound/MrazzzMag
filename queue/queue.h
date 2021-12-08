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

 	size_t size() const;

 private:
 	void clear();
 	void resize();

 	T* storage_ = { new T[MIN_SIZE] };
 	size_t capacity_ = { MIN_SIZE };
 	size_t first_ = {};
 	size_t last_ = {};
 };

 #include "queue.inl"