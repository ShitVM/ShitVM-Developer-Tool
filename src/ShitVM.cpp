#include <ShitVM.hpp>

#include <cstring>

#include <TlHelp32.h>

HANDLE ShitVMProcess;
std::string ShitVMProcessIdString = "ShitVM Process Id: (Not Found)";

bool FindShitVMProcess() noexcept {
	const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(snapshot, &entry)) {
		do {
			if (std::strcmp("ShitVM.exe", entry.szExeFile) != 0) continue;

			ShitVMProcess = OpenProcess(PROCESS_ALL_ACCESS, false, entry.th32ProcessID);
			ShitVMProcessIdString = "ShitVM Process Id: " + std::to_string(GetProcessId(ShitVMProcess));
			CloseHandle(snapshot);
			return true;
		} while (Process32Next(snapshot, &entry));
	}

	CloseHandle(snapshot);
	ShitVMProcessIdString = "ShitVM Process Id: (Not Found)";
	return false;
}

bool CheckShitVMProcessValid() noexcept {
	if (ShitVMProcess) {
		DWORD exitCode;
		GetExitCodeProcess(ShitVMProcess, &exitCode);
		if (exitCode == STILL_ACTIVE) return true;
		else {
			ShitVMProcess = nullptr;
			ShitVMProcessIdString += "(Dead)";
			return false;
		}
	}
	return true;
}
