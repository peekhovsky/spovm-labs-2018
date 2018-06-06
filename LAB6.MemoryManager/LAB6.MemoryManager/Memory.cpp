#include "stdafx.h"
#include "Memory.h"


Memory::Memory() {

	//allocate memory for buffer
	buffer = new char[BUFFER_SIZE];

	//allocate memory for page table
	pages = new Page[BUFFER_SIZE/PAGE_SIZE];

	//fulfill this table 
	for (int i = 0; i < PAGE_AMOUNT; i++) {
		pages[i].number = i;
		pages[i].offset = 0;
	}
}


Memory::~Memory() {
	delete[] buffer;
	delete[] pages;
}


Object* Memory::malloc(int alloc_size) {
	
	Object* object = new Object;

	for (int i = 0; i < PAGE_AMOUNT; i++) {

		bool is_bizy[PAGE_SIZE] = { false };

		for (int j = 0; j < pages[i].objects.size; j++) {
			int offset = pages[i].objects[j]->page_offset;
			int size = pages[i].objects[j]->size;
			
			for (int k = offset; k < size; k++) {
				is_bizy[k] = true;
			}
		}

		int peace_size = 0; 
		for (int j = 0; j < PAGE_AMOUNT; j++) {
			if (is_bizy[j] == true) {
				peace_size++;
			}
		}
		

		if ((PAGE_SIZE - pages[i].offset) > 0) {

			obj

			pages[i].offset += alloc_size;
			object->page_index = i;
			object->page_offset = pages[i].offset;
			object->size = alloc_size;
			return object;
		} 
	}
	return nullptr;
}

void Memory::free(Object* object) {
	int i = object->page_index;

	pages[object->page_index

}

