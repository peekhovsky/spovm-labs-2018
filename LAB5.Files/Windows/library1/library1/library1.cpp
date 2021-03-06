// Dll1.cpp: определяет экспортированные функции для приложения DLL.
#include "stdafx.h"

#define NUM_OF_SYMBOLS 10
#define AMOUNT_OF_FILES 15
#define STR_SIZE 64
#define MAX_FILESIZE 65535

char file_str[MAX_FILESIZE];
HANDLE event_output;
HANDLE event_input;
HANDLE event_close;


extern "C" __declspec(dllexport) void read() {
	//create events
	event_output = CreateEvent(NULL, false, false, NULL);
	event_input = CreateEvent(NULL, false, true, NULL);
	event_close = CreateEvent(NULL, false, false, NULL);

	char files[AMOUNT_OF_FILES][STR_SIZE] = {
		{ "file1.txt" },
		{ "file2.txt" },
		{ "file3.txt" },
		{ "file4.txt" },
		{ "file5.txt" },
		{ "file6.txt" },
		{ "file7.txt" },
		{ "file8.txt" },
		{ "file9.txt" },
		{ "file10.txt" },
		{ "file11.txt" },
		{ "file12.txt" },
		{ "file13.txt" },
		{ "file14.txt" },
		{ "file15.txt" }
	};

	//settings
	char buffer[NUM_OF_SYMBOLS + 1];
	DWORD bytes_in_file;
	OVERLAPPED overlapped;
	overlapped.hEvent = CreateEvent(NULL, true, true, NULL);

	//reading
	for (int i = 0; i < AMOUNT_OF_FILES; i++) {
		WaitForSingleObject(event_input, INFINITE);
		overlapped.Offset = 0;
		overlapped.OffsetHigh = 0;
		strcpy_s(file_str, sizeof(file_str), "");
		HANDLE handler_file = CreateFile(files[i], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

		do {
			memset(buffer, '\0', sizeof(buffer));
			if (ReadFile(handler_file, buffer, NUM_OF_SYMBOLS, &bytes_in_file, &overlapped) == 0) {
				break;
			}
			//wait for asin reading of file
			WaitForSingleObject(overlapped.hEvent, INFINITE);
			strcat_s(file_str, sizeof(file_str), buffer);
			overlapped.Offset += bytes_in_file;

		} while (bytes_in_file == NUM_OF_SYMBOLS);
		SetEvent(event_output);
		CloseHandle(handler_file);
	}
	SetEvent(event_close);
	CloseHandle(overlapped.hEvent);

}

extern "C" __declspec(dllexport) void write() {
	//create result file
	TCHAR file[STR_SIZE] = "result.txt";
	HANDLE handler_file = CreateFile(file, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	DWORD bytes_in_file;
	OVERLAPPED overlapped;

	overlapped.hEvent = CreateEvent(NULL, true, true, NULL);
	overlapped.Offset = 0;
	overlapped.OffsetHigh = 0;

	while (true) {

		if (WaitForSingleObject(event_output, 0) == WAIT_OBJECT_0) {

			WriteFile(handler_file, &file_str, strlen(file_str), &bytes_in_file, &overlapped);

			WaitForSingleObject(overlapped.hEvent, INFINITE);
			overlapped.Offset += bytes_in_file;
			SetEvent(event_input);
		}
		if (WaitForSingleObject(event_close, 0) == WAIT_OBJECT_0) {
			break;
		}
	}

	CloseHandle(handler_file);
	CloseHandle(event_input);
	CloseHandle(event_output);
	CloseHandle(event_close);
}
