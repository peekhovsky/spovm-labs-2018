#include "stdafx.h"
#include "Standart.h"

char* get_random_string(const int length) {
	static const char characters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	char* str = new char[length + 1];

	for (int i = 0; i < length; i++) {
		str[i] = characters[rand() % (sizeof(characters) - 1)];
	}
	str[length] = '\0';
	return str;
}
