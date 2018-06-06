#pragma once
#include <cstdlib>
#include <iostream>
#include <vector>



struct Object {
	char* pointer;
	int size;
	int page_index;
	int page_offset;
};

struct Page {
	int number;
	int offset;
	std::vector<Object*> objects;
};

class Memory {
public:
	static const int BUFFER_SIZE = 10000;
	static const int PAGE_SIZE = 100;
	static const int PAGE_AMOUNT = BUFFER_SIZE/PAGE_SIZE;



	char* buffer;
	Page* pages;
	
	//Constructor
	Memory();
	~Memory();

	Object* malloc(int size);
	void free(Object* object);
};

