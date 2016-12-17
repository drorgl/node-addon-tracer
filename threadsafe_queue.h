#ifndef _THREADSAFE_QUEUE_H_
#define _THREADSAFE_QUEUE_H_

#include <queue>
#include <mutex>
#include <atomic>

template<typename T>
class threadsafe_queue
{
private:
	std::queue<T> _queue;
	mutable std::mutex m;
	std::atomic_uint _queue_length;
public:
	threadsafe_queue() {}

	void enqueue(T item)
	{
		std::lock_guard<std::mutex> lock(m);
		_queue.push(item);
		_queue_length++;
	}

	bool dequeue(T &itemref)
	{
		std::lock_guard<std::mutex> lock(m);

		bool val = false;
		if (!_queue.empty()){
			_queue_length--;
			T item = _queue.front();
			itemref = item;
			_queue.pop();
			val = true;
		}
		return val;
	}
	
	unsigned int length() {
		return _queue_length;
	}

	void clear() {
		std::lock_guard<std::mutex> lock(m);
		std::queue<T> empty;
		std::swap(_queue, empty);
	}
};


#endif