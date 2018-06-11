#pragma once
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct Object {
	char* pointer;
	int size;
	int page_index;
	int page_offset;
};

struct Page {
	int number;
	char* offset;
	vector<Object*> objects;
};

class Memory {
public:
	static const int BUFFER_SIZE = 20;
	static const int BUFFER_AMOUNT = 2;
	static const int PAGE_SIZE = 10;
	static const int PAGE_AMOUNT = (BUFFER_SIZE / PAGE_SIZE) * BUFFER_AMOUNT;
	static const int PAGE_AMOUNT_IN_BUFFER = (BUFFER_SIZE / PAGE_SIZE);

	int current_buffer;

	char* buffer;
	vector<Page> pages;
	
	//Constructor
	Memory();
	~Memory();

	void show_page(int);
	void show_object(Object* object);

	Object* malloc(int size);
	void free(Object* object);

	void write_page_to_disk(int);

	void change_buffer();
	void change_buffer(int);

	
	void set_value(Object* object, string value);
	char* get_value(Object* object);
};

