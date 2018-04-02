// LAB1Processes.cpp : Defines the entry point for the console application.


/*
Вариант 12
Реализовать взаимодействие студента и преподавателя. Студент может сдавать
определенное количество лабораторных по какому - либо предмету (минимум два).
Преподаватель может как принять лабораторные, так и проинформировать студента
об отсутствии времени на прием лабораторных определенного предмета либо об
отсутствии этого предмета в нагрузке. Состояние занятости преподавателя должно
сохраняться.
*/



#include "stdafx.h"
#include <cstdlib>
#include <cstdio>
#include <windows.h>
#include <conio.h>
#include<iostream>
#include<string>
#include<climits>
#include <wchar.h>

using namespace std;

#define CHILD_PROCESS_LOCATION "F:\\Documents\\System Programming\\LAB1.ChildProcess\\Debug\\LAB1.ChildProcess.exe"
#define CHAR_SIZE 100


int getInt(int min, int max)
{
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


int main(int args, char** argv)
{

	
	cout << "How many labs do you want to pass?" << endl;
	int lab_count = getInt(1, INT_MAX);

	LPWSTR subjects = new TCHAR[CHAR_SIZE];
	subjects[0] = '\0';



	cout << "Enter names of subject that you want to pass: " << endl;

	for (int i = 0; i < lab_count; i++) 
	{
		cout << "Subject " <<i<<": "<< endl;
		rewind(stdin);
		LPWSTR temp = new TCHAR[CHAR_SIZE];
		fgetws(temp, CHAR_SIZE, stdin);
		

		for (int i = 0; temp[i] != '\0'; i++)
		{
			if (temp[i] == '\n') temp[i] = ' ';
		}

		wcscat_s(subjects, CHAR_SIZE, temp);
	}
	
	//wcout << subjects;


	//settings
	STARTUPINFO start_up_info;
	PROCESS_INFORMATION process_information;

	ZeroMemory(&start_up_info, sizeof(start_up_info));
	start_up_info.cb = sizeof(start_up_info);
	ZeroMemory(&process_information, sizeof(process_information));


	/*//create process
	if (!CreateProcess(TEXT("..."),		//file location
		NULL,							// Command line
		NULL,							// Process handle not inheritable
		NULL,							// Thread handle not inheritable
		FALSE,							// Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,				//
		NULL,							// Use parent's environment block
		NULL,							// Use parent's starting directory 
		&start_up_info,					// Pointer to STARTUPINFO structure
		&process_infornation)			// Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("CreateProcess failed (%d)\n", GetLastError());
		_getch();
		return -1;
	}
	*/
	//create process
	if (!CreateProcess(TEXT(CHILD_PROCESS_LOCATION), subjects, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &start_up_info, &process_information))
	{
		printf("CreateProcess failed (%d)\n", GetLastError());
		_getch();
		return -1;
	}

	SYSTEMTIME lt;

	cout << endl;
	while (WaitForSingleObject(process_information.hProcess, 50))
	{
		GetLocalTime(&lt);
		printf("%02d:%02d:%02d p\r", lt.wHour, lt.wMinute, lt.wSecond);
	}
	cout << endl << endl;

	DWORD exit_code = 0;
	DWORD result = GetExitCodeProcess(process_information.hProcess, &exit_code);

	wcout <<"Professor can get "<< exit_code <<" labs."<<endl;
	_getch();

	CloseHandle(process_information.hProcess);
	
	return 0;
  
}

