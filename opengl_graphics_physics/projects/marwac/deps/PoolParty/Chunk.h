#pragma once
#include <stdlib.h> 

template <typename T, int length>
class Chunk
{
public:
	Chunk(){}
	~Chunk(){}
	
	T data[length];
	Chunk<T,length>* next = nullptr;
	
	int current_free_index = 0;
	int data_size = length;

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

		void operator delete(void* p)
	{
		_mm_free(p);
	}
private:
	//Chunk();
};
