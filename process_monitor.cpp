#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <locale>
#include <stdexcept>

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <tlhelp32.h>

struct ProcessInfo {
    DWORD pid;
    std::wstring name;
};

std::vector<ProcessInfo> getProcesses() {
    std::vector<ProcessInfo> processes;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return processes;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    if (!Process32FirstW(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return processes;
    }

    do {
        processes.push_back(ProcessInfo{ pe32.th32ProcessID, std::wstring(pe32.szExeFile) });
    } while (Process32NextW(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return processes;
}

void printProcesses(const std::vector<ProcessInfo>& procs) {
    constexpr int pidWidth = 10;
    constexpr int nameWidth = 40;

    std::wcout << std::left << std::setw(pidWidth) << L"PID"
               << std::setw(nameWidth) << L"Process Name" << L"\n";
    std::wcout << std::wstring(pidWidth + nameWidth, L'-') << L"\n";

    for (const auto& p : procs) {
        std::wstring truncatedName = p.name.substr(0, nameWidth - 1);
        std::wcout << std::setw(pidWidth) << p.pid
                   << std::setw(nameWidth) << truncatedName << L"\n";
    }

    std::wcout << L"\nTotal Processes: " << procs.size() << L"\n";
}

#else

#include <dirent.h>
#include <fstream>
#include <algorithm>

struct ProcessInfo {
    int pid;
    std::string name;
};

std::vector<ProcessInfo> getProcesses() {
    std::vector<ProcessInfo> processes;
    DIR* proc = opendir("/proc");
    if (!proc) return processes;

    struct dirent* entry;
    while ((entry = readdir(proc)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            std::string pidStr = entry->d_name;
            if (std::all_of(pidStr.begin(), pidStr.end(), ::isdigit)) {
                int pid = std::stoi(pidStr);
                std::ifstream commFile("/proc/" + pidStr + "/comm");
                std::string name;
                if (commFile.is_open()) {
                    std::getline(commFile, name);
                    commFile.close();
                    processes.push_back({ pid, name });
                }
            }
        }
    }
    closedir(proc);
    return processes;
}

void printProcesses(const std::vector<ProcessInfo>& procs) {
    constexpr int pidWidth = 10;
    constexpr int nameWidth = 40;

    std::cout << std::left << std::setw(pidWidth) << "PID"
              << std::setw(nameWidth) << "Process Name" << "\n";
    std::cout << std::string(pidWidth + nameWidth, '-') << "\n";

    for (const auto& p : procs) {
        std::string truncatedName = p.name.substr(0, nameWidth - 1);
        std::cout << std::setw(pidWidth) << p.pid
                  << std::setw(nameWidth) << truncatedName << "\n";
    }

    std::cout << "\nTotal Processes: " << procs.size() << "\n";
}

#endif

int main() {
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(CP_UTF8);

    try {
        std::locale::global(std::locale(""));
        std::wcout.imbue(std::locale());
    } catch (const std::runtime_error&) {
    }
#endif

    auto processes = getProcesses();

    if (processes.empty()) {
        std::cerr << "No processes found or failed to retrieve.\n";
        return 1;
    }

    printProcesses(processes);

    return 0;
}
