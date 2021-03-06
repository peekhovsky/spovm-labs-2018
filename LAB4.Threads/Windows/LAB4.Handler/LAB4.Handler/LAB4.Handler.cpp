#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"

#include <Windows.h>

#include <conio.h>
#include <cstdio>

#include <iostream>
#include <vector>
#include <thread>
#include <ctime>
#include <string>
#include <mutex>

#define RANDOM_STRING_LENGTH 10

#define SUBJECT_QUANTITY 3
#define SUBJECTS "MATHS PHYSICS CHEMISTRY"

#define CHILD_PROCESS_LOCATION "C:\\Users\\rosti\\Documents\\GitHub\\systemprog-labs-2018\\LAB3.Semaphores\\Windows\\LAB3.Server\\Debug\\LAB3.Server.exe"
#define SERVER_LOCATION "C:\\Users\\rosti\\Documents\\GitHub\\systemprog-labs-2018\\LAB4.Threads\\Windows\\LAB3.Server\\Debug\\\LAB3.Server.exe"
#define PIPE_LOCATION "\\\\.\\pipe\\MyPipe"
#define CHAR_SIZE 100

#define SEMAPHORE_NAME "SEMAPHORE"

HANDLE read_semaphore;

std::mutex server_mutex;

std::string get_random_string(const int length) {
	static const char characters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	char* str = new char[length + 1];

	for (int i = 0; i < length; i++) {
		str[i] = characters[rand() % (sizeof(characters) - 1)];
	}
	str[length] = '\0';

	return std::string(str);
}


std::string execute_client() {
	char subjects[] = SUBJECTS;

	HANDLE pipe = CreateNamedPipe(TEXT(PIPE_LOCATION), PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 512, 512, 5000, NULL);

	read_semaphore = CreateSemaphore(NULL, 0, 1, SEMAPHORE_NAME);
	if (read_semaphore == NULL) {
		return "ERROR!";
	}

	//settings
	STARTUPINFO start_up_info;
	PROCESS_INFORMATION process_information;

	ZeroMemory(&start_up_info, sizeof(start_up_info));
	start_up_info.cb = sizeof(start_up_info);
	ZeroMemory(&process_information, sizeof(process_information));

	//create process
	CreateProcess(TEXT(SERVER_LOCATION), subjects,
		NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &start_up_info, &process_information);

	while (WaitForSingleObject(read_semaphore, 50));

	DWORD  cbRead;
	char   buffer[200] = { '\0' };
	ReadFile(pipe, buffer, 200, &cbRead, NULL);

	ConnectNamedPipe(pipe, NULL);

	while (WaitForSingleObject(process_information.hProcess, 50)) {

	}

	
	DWORD exit_code = 0;

	DWORD result = GetExitCodeProcess(process_information.hProcess, &exit_code);
	DisconnectNamedPipe(pipe);
	CloseHandle(process_information.hProcess);
	return std::string(buffer);
}


	void client_thread(std::mutex* end_mutex, std::string event_name) {

		std::string output_string = event_name + +": accepted subjects are " + execute_client();

		do {
			server_mutex.lock();
			system("cls");
			for (unsigned i = 0; output_string[i] != '\0', i < output_string.length(); i++) {
				std::cout << output_string[i];
				Sleep(100);
			}
			server_mutex.unlock();
			Sleep(400);
			if (end_mutex->try_lock()) {
				end_mutex->unlock();
			}
			else {
				break;
			}
		} while (true);
	}

struct ThreadMutex {
	std::thread* thread;
	std::mutex* mutex;
};

std::vector<ThreadMutex*> thread_stack;

void close_all_threads() {
	while (thread_stack.size() > 0) {
		ThreadMutex* thread_mutex = thread_stack.back();
		thread_stack.pop_back();
		thread_mutex->mutex->lock();
		break;
	}
}

BOOL ConsoleHandler(DWORD CEvent) {
	if (CEvent == CTRL_CLOSE_EVENT) {
		close_all_threads();
	}
	return TRUE;
}


int main() {
	srand((unsigned)time(NULL));
	
	//install exit handler
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE) {
		std::cout << "Error: cannot install exit handler!" << std::endl;
	}

	while (true) {

		char in_character;

		if (_kbhit()) {
			in_character = _getch();
		}
		else {
			in_character = '.';
		}

		switch (in_character) {

		//create new thread
		case '=':
		case '+': {
			std::mutex* end_mutex = new std::mutex;
			

			std::string thread_number = std::to_string(thread_stack.size());
			
			std::thread thr(client_thread, end_mutex, "Process " + thread_number + thread_number + thread_number + thread_number);
			thr.detach();
			ThreadMutex* thread_mutex = new ThreadMutex;
		
			thread_mutex->thread = &thr;
			thread_mutex->mutex = end_mutex;
			thread_stack.push_back(thread_mutex);
			break;
		}

		//delete last created thread
		case '-': {
			if (thread_stack.size() < 1) {
				break;
			}
			ThreadMutex* thread_mutex = thread_stack.back();
			thread_stack.pop_back();
			thread_mutex->mutex->lock();
			break;
		}
		//quit program
		case 'q': {
			close_all_threads();
			exit(0);
			break;
		}
		}

		Sleep(400);
	}

    return 0;
}

