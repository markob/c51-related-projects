// SCom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <Windows.h>
#include <tchar.h>


int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hCom;
	LPWCH  pcComPort = _T("COM1");
	DCB    dcb;

	DWORD bytesWritten;	
	BYTE  i1;

	hCom = CreateFile(pcComPort, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hCom) {
		printf("Unable to open COM1 port.\n");
		return 1;
	}

	GetCommState(hCom, &dcb);
	dcb.ByteSize = 8;
	SetCommState(hCom, &dcb);
	printf("COM1 baud rate is %d.\n", dcb.BaudRate);

	while (true) {
		printf("Enter value to be outputed at COM1 port:");
		scanf("%du", &i1);
		WriteFile(hCom, &i1, sizeof(i1), &bytesWritten, NULL);
	}

	CloseHandle(hCom);

	return 0;
}
