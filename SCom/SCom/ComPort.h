#pragma once

#include <Windows.h>

#include <string>

class CComPort
{
public:
	// Retrieves a list of all COM port devices in the system
	static std::string& Enumerate(void);

	CComPort(void);
	virtual ~CComPort(void);
private:
};
