#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <cstdlib>
#include <cstdio>
#include <windows.h>
#include <conio.h>
#include <iostream>
#include <string>
#include <climits>
#include <wchar.h>

using namespace std;


#define CHILD_PROCESS_LOCATION "C:\\Users\\rosti\\Documents\\GitHub\\systemprog-labs-2018\\LAB3.Semaphores\\Windows\\LAB3.Server\\Debug\\LAB3.Server.exe"
#define PIPE_LOCATION "\\\\.\\pipe\\MyPipe"
#define CHAR_SIZE 100

#define SEMAPHORE_NAME "SEMAPHORE"
HANDLE start_semaphore;
HANDLE print_semaphore;
HANDLE end_semaphore;
HANDLE read_semaphore;

int getInt(int min, int max)
{
	cin.clear();
	int res = 0;
	do
	{
		cin >> res;
		if ((res < min) || (res > max))
		{
			cout << "\nIncorrect input. Try again\n";
			rewind(stdin);
			cin.clear();
			continue;
		}
		else break;
	} while (true);
	return res;
}


void close_handles() {
	CloseHandle(start_semaphore);
	CloseHandle(print_semaphore);
	CloseHandle(end_semaphore);
}


BOOL WINAPI ConsoleHandler(DWORD CEvent) {
	switch (CEvent) {
	case CTRL_CLOSE_EVENT:
		close_handles();
	}
	return TRUE;
}


int server(char event_name[]) {

	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE) {
		printf("Unable to install handler!\n");
		return -1;
	}

	char* print_event_name = new char[11];
	char* end_event_name = new char[11];
	char* pipe_name = new char[11];

	strcpy(print_event_name, event_name);
	strcpy(end_event_name, event_name);
	strcpy(pipe_name, event_name);

	print_event_name[0] = '0';
	end_event_name[0] = '1';
	pipe_name[0] = '2';

	start_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, event_name);
	print_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, print_event_name);
	end_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, end_event_name);

	if (!start_semaphore || !print_semaphore || !end_semaphore) {
		cout << "Error! Cannot open semaphore!" << endl;
		//close_handles();
		//return 0;
	}

	do {
		system("cls");
		int index = WaitForSingleObject(start_semaphore, 0);
		Sleep(200);

		if (index == WAIT_OBJECT_0) {
			break;
		}
		else if ((WaitForSingleObject(print_semaphore, 0) == WAIT_OBJECT_0)) {
			for (int i = 0; event_name != '\0', i < 9; i++) {
				cout << event_name[i];
				Sleep(150);
			}
			ReleaseSemaphore(end_semaphore, 1, NULL);
		}
	} while (true);


	cout << "How many labs do you want to pass?" << endl;
	int lab_count = getInt(1, INT_MAX);

	char* subjects = new char[CHAR_SIZE];
	subjects[0] = '\0';

	cout << "Enter names of subject that you want to pass: " << endl;

	for (int i = 0; i < lab_count; i++) {
		cout << "Subject " << i << ": " << endl;
		rewind(stdin);
		char* temp = new char[CHAR_SIZE];
		fgets(temp, CHAR_SIZE, stdin);


		for (int i = 0; temp[i] != '\0'; i++) {
			if (temp[i] == '\n') temp[i] = ' ';
		}
		strcat_s(subjects, CHAR_SIZE, temp);
	}

	HANDLE pipe = CreateNamedPipe(TEXT(PIPE_LOCATION), PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 512, 512, 5000, NULL);

	cout << "AAA";

	cout << "BBB";
	read_semaphore = CreateSemaphore(NULL, 0, 1, SEMAPHORE_NAME);
	if (read_semaphore == NULL) {
		cout << "Cannot create semaphore!";
	}

	//settings
	STARTUPINFO start_up_info;
	PROCESS_INFORMATION process_information;

	ZeroMemory(&start_up_info, sizeof(start_up_info));
	start_up_info.cb = sizeof(start_up_info);
	ZeroMemory(&process_information, sizeof(process_information));

	//create process
	if (!CreateProcess(TEXT(CHILD_PROCESS_LOCATION), subjects, NULL, NULL,
		FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &start_up_info, &process_information))
	{
		cout << "CreateProcess failed: " << GetLastError() << endl;
		_getch();
		return -1;
	}

	SYSTEMTIME lt;

	cout << endl;

	while (WaitForSingleObject(read_semaphore, 50))
	{
		GetLocalTime(&lt);
		printf("%02d:%02d:%02d p\r", lt.wHour, lt.wMinute, lt.wSecond);
	}
	cout << endl;

	DWORD  cbRead;
	char   buffer[200] = { '\0' };
	ReadFile(pipe, buffer, 200, &cbRead, NULL);

	cout << cbRead << endl;
	cout << "Professor can assist this: " << buffer << endl;

	ConnectNamedPipe(pipe, NULL);

	while (WaitForSingleObject(process_information.hProcess, 50))
	{
		GetLocalTime(&lt);
		printf("%02d:%02d:%02d p\r", lt.wHour, lt.wMinute, lt.wSecond);
	}
	cout << endl << endl;

	DWORD exit_code = 0;
	DWORD result = GetExitCodeProcess(process_information.hProcess, &exit_code);

	cout << "Professor can get " << exit_code << " labs." << endl;
	_getch();

	DisconnectNamedPipe(pipe);
	CloseHandle(process_information.hProcess);
	close_handles();

	return 0;
