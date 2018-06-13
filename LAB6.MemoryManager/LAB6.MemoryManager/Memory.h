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

	static const int BUFFER_SIZE = 40;
	static const int BUFFER_AMOUNT = 4;
	static const int PAGE_SIZE = 10;
	static const int PAGE_AMOUNT = (BUFFER_SIZE / PAGE_SIZE) * BUFFER_AMOUNT;
	static const int PAGE_AMOUNT_IN_BUFFER = (BUFFER_SIZE / PAGE_SIZE);

	int current_buffer_number;
	char* buffer;
	vector<Page> pages;

public:
	//Constructor
	Memory();
	~Memory();

	void show_page(int);
	void show_object(Object* object);

	Object* malloc(int size);
	void free(Object* object);

	void change_buffer();
	void change_buffer(int);
	
	void set_value(Object* object, string value);
	char* get_value(Object* object);

	char* get_buffer();

	static int get_page_size();
	static int get_page_amount();
};

