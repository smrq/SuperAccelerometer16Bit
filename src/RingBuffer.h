#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

template <class T>
class RingBuffer
{
	private:
		T *buffer;
		int size;
		int cursor;

	public:
		RingBuffer(int size);
		~RingBuffer();
		T push(T value);
};

template <class T>
RingBuffer<T>::RingBuffer(int size)
	: size(size), cursor(0)
{
	buffer = new T[size];
}

template <class T>
RingBuffer<T>::~RingBuffer()
{
	delete[] buffer;
}

template <class T>
T RingBuffer<T>::push(T value)
{
	cursor = (cursor + 1) % size;
	T overwrittenValue = buffer[cursor];
	buffer[cursor] = value;
	return overwrittenValue;
}

#endif
