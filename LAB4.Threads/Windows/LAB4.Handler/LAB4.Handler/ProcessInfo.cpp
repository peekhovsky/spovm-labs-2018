#include "stdafx.h"
#include "ProcessInfo.h"


ProcessInfo::ProcessInfo(string location) {
	this->location = location;
	this->printing = false;

	//event names
	event_name.assign(get_random_string(EVENT_NAME_SIZE));
	print_event_name = event_name;
	end_print_event_name = event_name;

	print_event_name[0] = '0';
	end_print_event_name[0] = '1';

	//initialization of pid and startup
	ZeroMemory(&start_up_info, sizeof(start_up_info));
	start_up_info.cb = sizeof(start_up_info);
	ZeroMemory(&process_information, sizeof(process_information));
	create_event();
}

ProcessInfo::~ProcessInfo() {
	delete_process();
}

STARTUPINFO ProcessInfo::get_start_up_info() {
	return start_up_info;
}

PROCESS_INFORMATION ProcessInfo::get_process_information() {
	return process_information;
}

int ProcessInfo::create_process() {
	thr(server, event_name);
	
	/*
	int index = CreateProcess(TEXT(location.c_str()),
			LPSTR(event_name.c_str()), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &start_up_info, &process_information);

	if (index <= 0) {
		cout << "Create Process failed, error code: " << GetLastError() << "." << endl;
		return -1;
	}
	else {
		cout << "New Process has been created, str: ";
		cout << event_name << endl;
		return 0;
	}
	*/
	
}

void ProcessInfo::delete_process() {
	int info = 0;
	TerminateProcess(process_information.hProcess, info);
	CloseHandle(process_information.hProcess);
	CloseHandle(process_information.hThread);
	CloseHandle(start_semaphore);
	CloseHandle(print_semaphore);
	CloseHandle(end_print_semaphore);
	cout << "Process has been stopped. Code: " << info << endl;
}


void ProcessInfo::create_event() {
	start_semaphore		= CreateSemaphore(NULL, 0, 1, TEXT(event_name.c_str()));
	print_semaphore		= CreateSemaphore(NULL, 0, 1, TEXT(print_event_name.c_str()));
	end_print_semaphore = CreateSemaphore(NULL, 0, 1, TEXT(end_print_event_name.c_str()));

	/*
	start_event		= CreateEvent(NULL, FALSE, FALSE, TEXT(event_name.c_str()));
	print_event		= CreateEvent(NULL, FALSE, FALSE, TEXT(print_event_name.c_str()));
	end_print_event	= CreateEvent(NULL, FALSE, FALSE, TEXT(end_print_event_name.c_str()));
	*/
}

void ProcessInfo::enable_event() {
	ReleaseSemaphore(start_semaphore, 1, NULL);
}

void ProcessInfo::set_print_event() {
	printing = true;
	ReleaseSemaphore(print_semaphore, 1, NULL);
}

void ProcessInfo::reset_end_print_event() {
	printing = false;
	WaitForSingleObject(end_print_semaphore, 0);
}


bool ProcessInfo::is_running() {
	DWORD exit_code = 0;
	GetExitCodeProcess(process_information.hProcess, &exit_code);
	if (exit_code == STILL_ACTIVE) {
		return true;
	}
	else {
		return false;
	}
}

bool ProcessInfo::is_printing() {
	return printing;
}

void ProcessInfo::set_printing(bool value) {
	printing = value;
}

