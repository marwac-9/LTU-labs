#pragma once
#include <stdlib.h> 
#include "Chunk.h"

template <typename T, int chunk_length = 5>
class PoolParty
{
public:
	PoolParty(){}
	~PoolParty(){
		
		C* curChunk = firstChunk;
		for (int i = 0; i < init_number_of_chunks; i++)
		{
			C* nextChunk = curChunk->next;
			delete curChunk;
			curChunk = nextChunk;
		}
	}
	
	void CreatePoolParty(){
		C* previous_chunk = new C;
		firstChunk = previous_chunk;
		this->current_free_chunk = previous_chunk;
		
		for(int i = 0; i < init_number_of_chunks; ++i)
		{
			C* next_chunk = new C;
			previous_chunk->next = next_chunk;
			previous_chunk = next_chunk;
		}
	}
	T* PoolPartyAlloc(){
		if(this->current_free_chunk->current_free_index < this->current_free_chunk->data_size){
			T* element = &this->current_free_chunk->data[this->current_free_chunk->current_free_index];
			this->current_free_chunk->current_free_index++;
			return element;
		}
		else if(this->current_free_chunk->next != nullptr){
			this->current_free_chunk = this->current_free_chunk->next;
			T* element = &this->current_free_chunk->data[this->current_free_chunk->current_free_index];
			this->current_free_chunk->current_free_index++;
			return element;
		}
		else{
			C* next_chunk = new C;
			this->current_free_chunk->next = next_chunk;
			this->current_free_chunk = next_chunk;
			init_number_of_chunks++;
			T* element = &this->current_free_chunk->data[this->current_free_chunk->current_free_index];
			this->current_free_chunk->current_free_index++;
			return element;
		}
	}

private:
	typedef Chunk<T, chunk_length> C;
	C* current_free_chunk;
	int init_number_of_chunks = 100;
	C* firstChunk;
	//next free chunk
};

