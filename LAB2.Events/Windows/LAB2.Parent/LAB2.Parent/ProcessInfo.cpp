#include "stdafx.h"
#include "ProcessInfo.h"


ProcessInfo::ProcessInfo(char* str) {
	
	is_printing = false;
	event_name = new char[10];
	
	print_event_name = new char[10];
	
	end_event_name = new char[10];

	get_random_string(event_name, 10);

	strcpy(print_event_name, event_name);
	strcpy(end_event_name, event_name);

	print_event_name[0] = '0';
	end_event_name[0] = '1';

	location = str;
	init();
}

ProcessInfo::~ProcessInfo() {
	delete_process();
}

void ProcessInfo::init() {
	ZeroMemory(&start_up_info, sizeof(start_up_info));
	start_up_info.cb = sizeof(start_up_info);
	ZeroMemory(&process_information, sizeof(process_information));
	create_event();
}

STARTUPINFO ProcessInfo::get_start_up_info() {
	return start_up_info;
}

PROCESS_INFORMATION ProcessInfo::get_process_information() {
	return process_information;
}

int ProcessInfo::create_process() {
	
	int index = CreateProcess(
		TEXT("C:\\Users\\rosti\\Documents\\GitHub\\systemprog-labs-2018\\LAB2.Events\\Windows\\LAB2.Child1\\Debug\\LAB2.Child1.exe"),
			TEXT(event_name), NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &start_up_info, &process_information);

	if (index <= 0) {
		cout << "Create Process failed, error code: " << GetLastError() << "." << endl;
		_getch();
		return -1;
	}
	else {
		cout << "New Process has been created, str: ";
		//char buffer[LPWSTR_SIZE + 1] = { '\0' };
		//wcstombs(buffer, event_name, LPWSTR_SIZE);
		cout << event_name << endl;
		return 0;
	}
	
}

void ProcessInfo::delete_process() {
	int info = 0;
	TerminateProcess(process_information.hProcess, info);
	CloseHandle(process_information.hProcess);
	CloseHandle(process_information.hThread);
	cout << "Process has been stopped. Code: " << info << endl;
}

int ProcessInfo::is_running() {
	DWORD exit_code = 0;
	GetExitCodeProcess(process_information.hProcess, &exit_code);
	if (exit_code == STILL_ACTIVE) {
		return true;
	}
	else {
		return false;
	}
}

void ProcessInfo::create_event() {
	start_event = CreateEvent(NULL, FALSE, FALSE, TEXT(event_name));
	print_event = CreateEvent(NULL, FALSE, FALSE, TEXT(print_event_name));
	end_event = CreateEvent(NULL, FALSE, FALSE, TEXT(end_event_name));
}

void ProcessInfo::enable_event() {

	SetEvent(start_event);
}

void ProcessInfo::enable_print_event() {
	is_printing = true;
	SetEvent(print_event);
}


HANDLE ProcessInfo::get_process_handle() {
	return process_information.hProcess;
}