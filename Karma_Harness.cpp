/*******************************************************************************
* File:        Karma_Harness.cpp
* Author:
* Description: Karma Shellcode Test Harness 
* License:     MIT License
*
* This file is part of Karma Test Harness.
*
* Version:     0.1
*******************************************************************************/

#include <iostream>
#include <vector>
#include <fstream>
#include <windows.h>
#include <tchar.h>
#include "memory_tests.h"
#include "http_connect.h"

#define BUF_SIZE 0

typedef void (*shellcodeFunc)();

int ShellcodeExec(const char* shellcodeFile)
{
	shellcodeFunc shellcode;

	// Open the file
	std::ifstream file(shellcodeFile, std::ios::binary | std::ios::ate);
	if (!file.is_open())
	{
		std::cout << "Could not open shellcode file. Error code: " << GetLastError() << std::endl;
		return 1;
	}

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	if (!file.read(buffer.data(), size))
	{
		std::cout << "Could not read shellcode file. Error code: " << GetLastError() << std::endl;
		return 1;
	}

	file.close();

	// Allocate memory for the shellcode with execute, read and write permissions
	void* exec_mem = VirtualAlloc(NULL, size*2, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (exec_mem == NULL)
	{
		std::cout << "Could not allocate memory for shellcode. Error code: " << GetLastError() << std::endl;
		return 1;
	}

	// Copy the shellcode into the allocated memory
	memcpy(exec_mem, buffer.data(), size);
	shellcode = (shellcodeFunc)exec_mem;

	printf("SHELLCODE ADDRESS = %p, SIZE = %lld\n", shellcode, size);

	// Try to execute the shellcode:
	shellcode();  // Visual Studio tip: breakpoint here, switch to disassembly view, then step-into.

	// Clean up (but you'll probably never reach here because shellcode will 
	// exit and terminate the thread):
	VirtualFree(exec_mem, 0, MEM_RELEASE);

	return 0;
}


int main(int argc, char* argv[])
{
	std::string protectionOption;
	std::string shellcodePath; // = "shellcodex64_bind_meterpreter.raw";  // default path

	if (argc > 1) {
		// Iterate through the command-line arguments
		for (int i = 1; i < argc; ++i) {
			std::string arg = argv[i];

			// Check if the argument is "--protection"
			if (arg == "--protection") {
				protectionOption = "True";
			}
			// Check if the argument is "--shellcode-path"
			else if (arg == "--shellcode-path" && i + 1 < argc) {
				shellcodePath = argv[++i];  // increment i to get the next argument
			}
		}
	}

	if (!protectionOption.empty()) {
		printf("[HARNESS] Running Process with KARMA Protection\n");
		HMODULE hMod = LoadLibraryA("C:\\Windows\\System32\\karma.dll");
		if (hMod == NULL)
		{
			std::cout << "Could not load protection."
				<< GetLastError() << std::endl;
			return -1;
		}
	}
	else
	{
		printf("[HARNESS] Running Process without Protection\n");
	}
	
	printf("[HARNESS] Sleeping for 1 seconds\n");
	Sleep(1000);
	
	OpenHttpsConnection(L"www.google.com");
	OpenHttpsConnection(L"www.cnn.com");
	OpenHttpsConnection(L"www.x.com");
	OpenHttpsConnection(L"www.facebook.com");
	OpenHttpsConnection(L"www.youtube.com");
	OpenHttpsConnection(L"www.amazon.com");

	//TestMemoryAllocationAndProtection();

	printf("[HARNESS] Triggering vulnerable code\n");
	printf("[HARNESS] Executing Shellcode - %s\n", shellcodePath.c_str());
	ShellcodeExec((char *)shellcodePath.c_str());  

	return 0;
}


