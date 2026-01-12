// manually written DynamicList container, to replace std::vector

#pragma once

template<typename T>
class DynamicList
{
private:
	const static int DEFAULT_CAPACITY = 100;

	int capacity_;
	int size_;
	T* array_;
	bool resizeable_;

	void growIfNeeded() // automatically double capacity if full
	{
		if (size_ == capacity_)
		{
			if (!resizeable_) return;
			capacity_ *= 2;
			T* newArray = new T[capacity_];
			for (int i = 0; i < size_; i++)
				newArray[i] = array_[i];
			delete[] array_;
			array_ = newArray;
		}
	}

public:
	DynamicList(int c = DEFAULT_CAPACITY, bool r = true) : capacity_(c), size_(0), array_(new T[c]), resizeable_(r) { };

	DynamicList(const DynamicList& other) : capacity_(other.capacity_), size_(other.size_), array_(new T[other.capacity_])
	{
		for (int i = 0; i < size_; i++)
			array_[i] = other.array_[i];
	}

	~DynamicList()
	{
		delete[] array_;
	}

	bool empty() const
	{
		return size_ == 0;
	}

	int size() const
	{
		return size_;
	}

	int capacity() const
	{
		return capacity_;
	}

	void insert(T element, int pos)
	{
		if (pos < 0 || pos > size_) return;
		growIfNeeded();
		for (int i = size_ - 1; i >= pos; i--)
			array_[i + 1] = array_[i];
		array_[pos] = element;
		size_++;
	}

	void erase(int pos)
	{
		if (pos < 0 || pos >= size_) return;
		for (int i = pos; i < size_ - 1; i++)
			array_[i] = array_[i + 1];
		size_--;
	}

	void clear()
	{
		size_ = 0;
	}

	void push(T element)
	{
		growIfNeeded();
		array_[size_++] = element;
	}

	T pop()
	{
		if (size_ == 0) return T();
		return array_[--size_];
	}

	void popMultiple(int n)
	{
		if (size_ - n < 0) { size_ = 0; return; }
		size_ -= n;
	}

	DynamicList& operator=(const DynamicList& other)
	{
		if (this == &other) return *this;
		delete[] array_;
		capacity_ = other.capacity_;
		size_ = other.size_;
		array_ = new T[capacity_];
		for (int i = 0; i < size_; i++)
			array_[i] = other.array_[i];
		return *this;
	}

	T& operator[](int index) // changing an element (returns a reference to original element in array) ex: list1[2] = 5;
	{
		return array_[index];
	}

	const T& operator[](int index) const // reading an element (returns a constant refernce to original element in array) ex: cout << list1[2];
	{
		return array_[index];
	}

	// these are needed to make container work with range-based for loop ex: for (int i : list1) { ... }
	T* begin() { return array_; }		// pointer to first element of array
	T* end() { return array_ + size_; } // pointer to one position after last element of array

	const T* begin() const { return array_; }
	const T* end() const { return array_ + size_; }
};
