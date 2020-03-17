#include <ShitVM.hpp>

#include <cstring>

#include <TlHelp32.h>

HANDLE ShitVMProcess;

bool FindShitVMProcess() noexcept {
	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(snapshot, &entry)) {
		do {
			if (std::strcmp("ShitVM.exe", entry.szExeFile) != 0) continue;

			ShitVMProcess = OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID);
			CloseHandle(snapshot);
			return true;
		} while (Process32Next(snapshot, &entry));
	}

	CloseHandle(snapshot);
	return false;
}