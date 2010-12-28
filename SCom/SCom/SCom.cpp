// SCom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Windows.h>
#include <tchar.h>

#include "ComPort.h"

#include <iostream>

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	CComPort comPort;

	comPort.Open(_T("COM1"));
	comPort.SetBaudRate(9600);

	// do somethimg interesting here
	char ch = '\0';
	do {
		// read character from the input
		cout << "Enter a character to send to COM port ('q' to exit): ";
		cin >> &ch;

		// try to send it to the COM port
		if (comPort.SendByte(ch)) {
			cout << "Failed to send a byte to the COM port" << endl;
			break;
		}

		// try to get one byte from the COM port
		if (comPort.RecvByte(reinterpret_cast<BYTE*>(&ch))) {
			cout << "Failed to receive a byte from the COM port" << endl;
			break;
		}

		// write the received character to the output
		cout << "Character '" << ch << "' has been received from the COM port" << endl;

	} while ('q' != ch);

	cout << "Close COM port" << endl;
	comPort.Close();

	return 0;
}
