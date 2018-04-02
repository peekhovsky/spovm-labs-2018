#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <conio.h>
#include <iostream>
#include <windows.h>
#include <stack>
#include <string>
#include <vector>

#define CHILD_PROCESS_LOCATION "C:\\Users\\rosti\\Documents\\GitHub\\systemprog-labs-2018\\LAB2.Events\\Windows\\LAB2.Child1\\Debug\\LAB2.Child1.exe"

using namespace std;



class ProcessInfo {
	
	STARTUPINFO start_up_info;
	PROCESS_INFORMATION process_information;
	LPCWSTR location;
  public:

	ProcessInfo(LPCWSTR str) {
		location = str;
		init();
	}

	~ProcessInfo() {
		delete_process();
	}

	void init() {
		ZeroMemory(&start_up_info, sizeof(start_up_info));
		start_up_info.cb = sizeof(start_up_info);
		ZeroMemory(&process_information, sizeof(process_information));
	}

	STARTUPINFO get_start_up_info() {
		return start_up_info;
	}

	PROCESS_INFORMATION get_process_information() {
		return process_information;
	}

	int create_process() {
		int index = CreateProcess(location, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &start_up_info, &process_information);
		
		if (index <= 0) {
			cout<<"Create Process failed, error code: "<<GetLastError()<<"."<<endl;
			_getch();
			return -1;
		} 
		else {
			cout << "New Process has been created."<<endl;
			return 0;
		}
	}

	void delete_process() {
		UINT info = 0;
		TerminateProcess(process_information.hProcess, info);
		CloseHandle(process_information.hProcess);
		CloseHandle(process_information.hThread);
		cout << "Process has been stopped. Code: " << info << endl;
	}

	int is_running() {
		DWORD exit_code = 0;
		GetExitCodeProcess(process_information.hProcess, &exit_code);
		if (exit_code == STILL_ACTIVE) {
			return true;
		}
		else {
			return false;
		}
	}
};

vector<ProcessInfo*> process_stack;

void delete_all_processes() {
	while (process_stack.size() > 0) {
		cout << endl << "Terminating of all running processes" << endl;
		ProcessInfo* process_info = process_stack.back();
		process_stack.pop_back();
		process_info->delete_process();
	}
	exit(0);
}

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	switch (CEvent) {
	case CTRL_CLOSE_EVENT:
		delete_all_processes();
	}
	return TRUE;
}


//The entry point for the console application.
int main(int args, char** argv) {
	
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE) {
		// unable to install handler... 
		// display message to the user
		printf("Unable to install handler!\n");
		return -1;
	}

	//vector<ProcessInfo*> process_stack;


	cout << "Enter to create new process: '+', to delete last created: '-', quit: 'q'" << endl;
	while (true) {
		
		char in_character;
		if (_kbhit()) {
			in_character = _getch();
		} 
		else {
			in_character = '.'; 
		}
	

		switch (in_character) {
			
		case 0x2B:
		case 0x3D: { //ascii '=' and '+' 
			cout << endl << "Create new process:" << endl;
			ProcessInfo* process_info = new ProcessInfo(TEXT(CHILD_PROCESS_LOCATION));
			if (process_info->create_process() == 0) {
				process_stack.push_back(process_info);
				//process_stack.push(process_info);
			}
			break;
		}

		case 0x2D:  { //ascii '-'	
			if (process_stack.size() > 0) {
				cout << endl<< "Terminating of last created process: " << endl;
				ProcessInfo* process_info = process_stack.back();
				process_stack.pop_back();
				process_info->delete_process();

			} 
			else {
				cout << endl <<"There are no running processes." << endl;
			}
			break;
		}

		case 0x71: { //ascii 'q'
			delete_all_processes();
			exit(0);
			break;
		}
		default: 
			cout<<".";
			break;
			
		}
	
		for (int i = 0; i < process_stack.size(); i++) {

			if (process_stack[i]->is_running() == false) {
				cout << endl <<"Process " << i << " was closed independently." << endl;
				process_stack.erase(process_stack.begin() + i);
			}
		}

		Sleep(400);

	}
    return 0;
}

