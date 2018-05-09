#pragma once

#include "stdafx.h"

#include <Windows.h>
#include <iostream>

#include "Standart.h"

#define CHILD_PROCESS_LOCATION "C:\\Users\\rosti\\Documents\\GitHub\\systemprog-labs-2018\\LAB2.Events\\Windows\\LAB2.Child1\\Debug\\LAB2.Child1.exe"
#define LPWSTR_SIZE 10 

using namespace std;


class ProcessInfo {
public:
	static int counter;
	STARTUPINFO start_up_info;
	PROCESS_INFORMATION process_information;
	char* location;
	char* event_name;
	
	char* print_event_name;
	char* end_event_name;

	bool is_printing;
	HANDLE start_event;
	HANDLE print_event;
	HANDLE end_event;
	void init();


	ProcessInfo(char* str);

	~ProcessInfo();

	STARTUPINFO get_start_up_info();

	PROCESS_INFORMATION get_process_information();

	int create_process();

	void delete_process();

	int is_running();

	void create_event();

	void enable_event();

	void enable_print_event();

	HANDLE get_process_handle();
};