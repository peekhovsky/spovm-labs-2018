#include "stdafx.h"
#include "Standart.h"

void get_random_string(char* &str, const int length) {
	static const char characters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	str = new char[length];

	for (int i = 0; i < length; i++) {
		str[i] = characters[rand() % (sizeof(characters) - 1)];
	}
	str[length - 1] = '\0';
}
