#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <Windows.h>
#include <conio.h>

#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <ctime>

#include "ProcessInfo.h"
#include "Standart.h"
#include "System.h"

using namespace std;

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

BOOL WINAPI ConsoleHandler(DWORD CEvent) {
	
	switch (CEvent) {
	case CTRL_CLOSE_EVENT:
		delete_all_processes();
	}
	return TRUE;
}


int main(int args, char** argv) {
	srand(time(NULL));

	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE) {
		printf("Unable to install handler!\n");
		return -1;
	}

	ProcessInfo* print_process = nullptr;
	int print_process_index = 0;

	cout << "Enter to create new process: '+', to delete last created: '-', quit: 'q'" << endl;
	while (true) {	
		
		ProcessInfo* performing_process = nullptr;
		char in_character;
		
		if (_kbhit()) {
			in_character = _getch();
		} 
		else {
			in_character = '.'; 
		}

		switch (in_character) {
			
		case '=':
		case '+': { //ascii '=' and '+' 
			ProcessInfo* process_info = new ProcessInfo(NULL);
	
			if (process_info->create_process() == 0) {
				process_stack.push_back(process_info);
			}
			break;
		}

		case '-':  { //ascii '-'	
			if (process_stack.size() > 0) {
				cout << "Terminating of last created process: " << endl;
				ProcessInfo* process_info = process_stack.back();
				process_stack.pop_back();
				process_info->delete_process();
			} 
			else {
				cout << "There are no running processes." << endl;
			}
			break;
		}

		case 'q': { //ascii 'q'
			exit(0);
			break;
		}

		default: 
			//cout<<".";
			break;	
		}
	

		if ((performing_process == nullptr) && (process_stack.size() > 0)) {
			performing_process = process_stack[0];
			performing_process->enable_event();
		}

		bool is_printing = false;

		for (int i = 0; i < process_stack.size(); i++) {
			
			if (process_stack[i]->is_running() == false) {
				cout <<  "Process " << i << " was closed independently." << endl;
				process_stack.erase(process_stack.begin() + i);
			}	
			else if (process_stack[i]->is_printing == true) {
				is_printing = true;
				if (WaitForSingleObject(process_stack[i]->end_event, 0) == WAIT_OBJECT_0) {
					ResetEvent(process_stack[i]->end_event);
					process_stack[i]->is_printing = false;
					if (i < (process_stack.size() - 1)) {
						process_stack[i + 1]->enable_print_event();
					}
					else {
						is_printing = false;
					}
				}
			}
		}

		if (is_printing == false && (process_stack.size() >= 2)) {
			process_stack[1]->enable_print_event();
			
		}

		if (performing_process != nullptr) {
			if (WaitForSingleObject(performing_process->get_process_handle(), 400) == NULL) {
				performing_process = nullptr;
			}
		}
		else {
			Sleep(400);
		}
	}
		
		
	return 0;		
}
