// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
//#ifndef SERIALPORT_H
#define SERIALPORT_H

#define ARDUINO_WAIT_TIME 2000
#define MAX_DATA_LENGTH 255

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

class SerialPort
{
private:
	HANDLE handler;
	bool connected;
	COMSTAT status;
	DWORD errors;
	//bool disconnected;
public:
	SerialPort(const char* portName);
	~SerialPort();

	int readSerialPort(char* buffer, unsigned int buf_size);
	bool writeSerialPort(char* buffer, unsigned int buf_size);
	bool isConnected();
	//bool notConnected();
};

//#endif // SERIALPORT_H

#if defined(_WIN32) || defined(_WIN64)

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <tchar.h>

#elif defined(MACOSX)

#else

#include <stdio.h>

#endif
