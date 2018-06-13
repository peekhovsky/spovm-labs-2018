#include "stdafx.h"
#include "Memory.h"


Memory::Memory() {
	//allocate memory for buffer
	buffer = new char[BUFFER_SIZE + 1];
	memset(buffer, '_', BUFFER_SIZE);
	buffer[BUFFER_SIZE] = '\0';
	
	for (int i = 0; i < BUFFER_AMOUNT; i++) {
		
		string file_name = to_string(i) + ".txt";
		ofstream file;
		file.open(file_name);

		if (file.is_open()) {
			for (int i = 0; i < BUFFER_SIZE; i++) {
				file << "\0";
			}
			file << endl;
		}
		else {
			cout << "Error: cannot create files!" << endl;
		}
		file.close();

	}

	current_buffer_number = 0;

	//allocate memory for page table
	for (int i = 0; i < PAGE_AMOUNT; i++) {
		Page page;
		pages.push_back(page);
	}

	//fulfill this table 
	for (int i = 0; i < PAGE_AMOUNT; i++) {
		pages[i].number = i;
		pages[i].offset = buffer + i * PAGE_SIZE;
	}
}


Memory::~Memory() {
	delete[] buffer;
	pages.clear();
}

void Memory::show_page(int index) {
	
	if (index < pages.size()) {
		cout << "Page number: " << pages[index].number << std::endl;
		cout << "Objects: " << endl;
		cout << endl;
		for (int i = 0; i < pages[index].objects.size(); i++) {
			show_object(pages[index].objects[i]);
		}
	}
}

void Memory::show_object(Object * object) {
	
	cout << "Object: " << endl;
	cout << "Page index: " << object->page_index << endl;
	cout << "Page offset: " << object->page_offset << endl;
	cout << "Object value: " << get_value(object) << endl;
	cout << "Object size: " << object->size << endl;
	cout << endl;
}

Object* Memory::malloc(int alloc_size) {
	
	Object* object = new Object;

	int p = 0;

	while (true) {
		for (int i = PAGE_AMOUNT_IN_BUFFER * current_buffer_number;
			i < PAGE_AMOUNT_IN_BUFFER * current_buffer_number + PAGE_AMOUNT_IN_BUFFER; i++) {

			bool is_bizy[PAGE_SIZE] = { false };

			for (int j = 0; j < pages[i].objects.size(); j++) {

				int offset = pages[i].objects[j]->page_offset;
				int size = pages[i].objects[j]->size;

				for (int k = offset; k < (offset + size); k++) {
					is_bizy[k] = true;
				}
			}

			int peace_size = 0;
			for (int j = 0; j <= PAGE_SIZE; j++) {

				if (is_bizy[j] != true) {

					if (peace_size >= alloc_size) {

						object->pointer = buffer + PAGE_SIZE * (i - (PAGE_AMOUNT_IN_BUFFER * current_buffer_number)) 
							+ (j - peace_size);
						object->page_index = i;
						object->page_offset = j - peace_size;
						object->size = alloc_size;

						pages[i].objects.push_back(object);
						return object;
					}
					peace_size++;
				}
				else {
					peace_size = 0;
				}
			}
		}

		if (p > BUFFER_AMOUNT) {
			cout << "Error: memory is empty!" << endl;
			break;
		}
		else {
			p++;
			cout << "Change buffer: from " << current_buffer_number << " to ";
			change_buffer();
			cout << current_buffer_number << endl;
		}
	}
	return nullptr;
}

void Memory::free(Object* object) {

	auto it = std::find(pages[object->page_index].objects.begin(), pages[object->page_index].objects.end(), object);

	if (it != pages[object->page_index].objects.end()) {
		pages[object->page_index].objects.erase(it); 
	}
}


void Memory::change_buffer() {

	//old buffer
	string file_name = to_string(current_buffer_number) + ".txt";
	ofstream old_buffer_file;
	old_buffer_file.open(file_name);

	if (old_buffer_file.is_open()) {
		old_buffer_file << buffer;
		old_buffer_file.write(buffer, BUFFER_SIZE);
	}
	old_buffer_file.close();

	//change number of buffer
	if (current_buffer_number == (BUFFER_AMOUNT - 1)) {
		current_buffer_number = 0;
	}
	else {
		current_buffer_number++;
	}

	//new buffer
	file_name = to_string(current_buffer_number) + ".txt";
	ifstream new_buffer_file(file_name);

	if (new_buffer_file.is_open()) {
		new_buffer_file.read(buffer, BUFFER_SIZE);
	}

	new_buffer_file.close();
}

void Memory::change_buffer(int new_buffer_index) {

	if (new_buffer_index >= BUFFER_AMOUNT) {
		cout << "Error: incorrent index of buffer!" << endl;
		return;
	}

	//old buffer
	string file_name = to_string(current_buffer_number) + ".txt";
	ofstream old_buffer_file;
	old_buffer_file.open(file_name);

	if (old_buffer_file.is_open()) {
		old_buffer_file << buffer;
	}
	old_buffer_file.close();

	current_buffer_number = new_buffer_index;

	//new buffer
	file_name = to_string(current_buffer_number) + ".txt";
	ifstream new_buffer_file(file_name);

	if (new_buffer_file.is_open()) {
		new_buffer_file.read(buffer, BUFFER_SIZE);
	}
	new_buffer_file.close();
}

void Memory::set_value(Object * object, string value) {
	if (current_buffer_number != (object->page_index / PAGE_AMOUNT_IN_BUFFER)) {
		cout << "Change buffer: from " << current_buffer_number << " to " << object->page_index / PAGE_AMOUNT_IN_BUFFER << endl;
		change_buffer(object->page_index / PAGE_AMOUNT_IN_BUFFER);
	}

	int offset = ((object->page_index % PAGE_AMOUNT_IN_BUFFER) * PAGE_SIZE);
	int j = 0;
	for (int i = object->page_offset + offset; (i < (object->page_offset + object->size + offset)) && (j < value.size()); i++) {
		buffer[i] = value[j];
		j++;
	}
	
}

char* Memory::get_value(Object* object) {

	if (current_buffer_number != (object->page_index / PAGE_AMOUNT_IN_BUFFER)) {
		cout << "Change buffer: from " << current_buffer_number << " to " << object->page_index / PAGE_AMOUNT_IN_BUFFER << endl;
		change_buffer(object->page_index / PAGE_AMOUNT_IN_BUFFER);
		
	}

	char* return_value = new char[object->size + 1];
	memset(return_value, '\0', object->size + 1);
	strncpy(return_value, object->pointer, object->size);
	return return_value;
}

char* Memory::get_buffer() {
	return buffer;
}

int Memory::get_page_size() {
	return PAGE_SIZE;
}

int Memory::get_page_amount() {
	return PAGE_AMOUNT;
}

