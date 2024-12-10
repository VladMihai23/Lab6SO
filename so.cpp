#include <iostream>
#include <vector>
#include <windows.h>
#include <sstream>

bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

void findPrimes(int start, int end, HANDLE writePipe) {
    std::ostringstream primes;
    for (int i = start; i < end; ++i) {
        if (isPrime(i)) {
            primes << i << " ";
        }
    }

    // Scrie numerele prime în pipe
    std::string primesStr = primes.str();
    DWORD bytesWritten;
    WriteFile(writePipe, primesStr.c_str(), primesStr.size() + 1, &bytesWritten, nullptr);

    CloseHandle(writePipe);
}

int main() {
    const int numProcesses = 10;
    const int range = 10000;
    const int chunkSize = range / numProcesses;

    HANDLE readPipes[numProcesses];
    HANDLE writePipes[numProcesses];
    PROCESS_INFORMATION procInfo[numProcesses];
    STARTUPINFO startInfo[numProcesses];

    for (int i = 0; i < numProcesses; ++i) {
        // Creează un pipe pentru comunicare
        HANDLE readPipe, writePipe;
        if (!CreatePipe(&readPipe, &writePipe, nullptr, 0)) {
            std::cerr << "Eroare la crearea pipe-ului!" << std::endl;
            return 1;
        }
        readPipes[i] = readPipe;
        writePipes[i] = writePipe;

        ZeroMemory(&startInfo[i], sizeof(STARTUPINFO));
        startInfo[i].cb = sizeof(STARTUPINFO);

        // Creează procesul copil
        std::ostringstream cmd;
        cmd << "child.exe " << (i * chunkSize) << " " << ((i + 1) * chunkSize) << " " << (uintptr_t)writePipe;

        if (!CreateProcess(
            nullptr,
            const_cast<LPSTR>(cmd.str().c_str()), // linia de comandă
            nullptr, nullptr, TRUE, 0, nullptr, nullptr, &startInfo[i], &procInfo[i])) {
            std::cerr << "Eroare la crearea procesului!" << std::endl;
            return 1;
        }

        // Închide capătul de scriere al pipe-ului în procesul părinte
        CloseHandle(writePipe);
    }

    // Citește rezultatele de la procesele copil
    for (int i = 0; i < numProcesses; ++i) {
        char buffer[4096];
        DWORD bytesRead;
        if (ReadFile(readPipes[i], buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
            buffer[bytesRead] = '\0'; // Null-terminate
            std::cout << "Numere prime de la procesul " << i << ": " << buffer << std::endl;
        }
        else {
            std::cerr << "Eroare la citirea din pipe pentru procesul " << i << "!" << std::endl;
        }
        CloseHandle(readPipes[i]);
        CloseHandle(procInfo[i].hProcess);
        CloseHandle(procInfo[i].hThread);
    }

    return 0;
}
