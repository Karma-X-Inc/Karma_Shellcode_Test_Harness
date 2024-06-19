#include <Windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>

void CreateTestFileWithRandomData(const std::string& filename, size_t size) {
    std::ofstream file(filename, std::ios::binary);
    std::vector<char> data(size);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (auto& byte : data) {
        byte = static_cast<char>(dis(gen));
    }

    if (file.is_open()) {
        file.write(data.data(), data.size());
    }
}

void TestMemoryAllocationAndProtection() {
    const size_t dataSize = 4096;

    // Create a file with random data
    CreateTestFileWithRandomData("testfile.bin", dataSize);
    
    // 1. Allocate Memory
    LPVOID allocMem = VirtualAlloc(NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    // Example with VirtualAlloc2 (Windows 10, version 1709+)
    // MEM_ADDRESS_REQUIREMENTS and MEM_EXTENDED_PARAMETER structures are used with VirtualAlloc2

    // 2. Protect Memory
    DWORD oldProtect;
    if (allocMem) {
        VirtualProtect(allocMem, 4096, PAGE_EXECUTE_READWRITE, &oldProtect);
        // Example with PAGE_EXECUTE_READWRITE, PAGE_NOACCESS, etc.
    }

    // 3. Lock Memory
    if (allocMem) {
        VirtualLock(allocMem, 4096);
    }

    // 4. Query Memory
    MEMORY_BASIC_INFORMATION mbi;
    if (allocMem) {
        VirtualQuery(allocMem, &mbi, sizeof(mbi));
        // Output memory information
    }

    // 5. File Operations
    std::ifstream file("testfile.bin", std::ios::binary);
    if (file.is_open() && allocMem) {
        file.read(static_cast<char*>(allocMem), 4096);
        file.close();
    }

    // 6. Free Memory
    if (allocMem) {
        VirtualFree(allocMem, 0, MEM_RELEASE);
    }

    // Additional code for VirtualAllocEx, VirtualFreeEx, VirtualProtectEx, VirtualQueryEx
    // These functions require a process handle, which you need to obtain as per your scenario.
}