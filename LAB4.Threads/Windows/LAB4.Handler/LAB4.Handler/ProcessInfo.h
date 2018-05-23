#pragma once

#include "stdafx.h"

#include <thread>
#include <Windows.h>
#include <iostream>
#include <String>
#include "Standart.h"
#include "Server.h"

#define EVENT_NAME_SIZE 10 

using namespace std;


class ProcessInfo {
	
	string location;

	string event_name;
	string print_event_name;
	string end_print_event_name;

	bool printing;

public:

	thread thr;

	//handlers
	HANDLE start_event;
	HANDLE print_event;
	HANDLE end_print_event;	

	HANDLE start_semaphore;
	HANDLE print_semaphore;
	HANDLE end_print_semaphore;

	STARTUPINFO start_up_info;
	PROCESS_INFORMATION process_information;

	//Constructor
	ProcessInfo(string location);
	~ProcessInfo();

	//get
	STARTUPINFO get_start_up_info();
	PROCESS_INFORMATION get_process_information();

	//processes
	int create_process();
	void delete_process();

	bool is_running();
	bool is_printing();
	void set_printing(bool value);

	//events
	void create_event();
	void enable_event();
	void set_print_event();
	void reset_end_print_event();
};