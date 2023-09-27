#include <Windows.h>
#include <iostream>
#include <string>

#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>

const char* COM_PORT = "COM8";  // Change to the desired COM port
const int BAUD_RATE = 115200;
const int BUFFER_SIZE = 256;

std::atomic<bool> shouldExit(false);

void ReadSerialData(HANDLE hSerial) {
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    while (!shouldExit) {
        if (ReadFile(hSerial, buffer, BUFFER_SIZE, &bytesRead, nullptr)) {
            if (bytesRead > 0) {
                // Print received data to the console
                std::cout.write(buffer, bytesRead);
                std::cout.flush();  // Ensure the output is immediately visible
            }
        }
    }
}

int main() {
    HANDLE hSerial;
    hSerial = CreateFileA(COM_PORT, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening serial port." << std::endl;
        return 1;
    }

    DCB serialConfig = { 0 };
    serialConfig.DCBlength = sizeof(serialConfig);

    if (!GetCommState(hSerial, &serialConfig)) {
        std::cerr << "Error getting serial port state." << std::endl;
        CloseHandle(hSerial);
        return 1;
    }

    serialConfig.BaudRate = BAUD_RATE;
    serialConfig.ByteSize = 8;
    serialConfig.StopBits = ONESTOPBIT;
    serialConfig.Parity = NOPARITY;

    if (!SetCommState(hSerial, &serialConfig)) {
        std::cerr << "Error setting serial port state." << std::endl;
        CloseHandle(hSerial);
        return 1;
    }

    // Create a thread for asynchronous serial data reading
    std::thread serialThread(ReadSerialData, hSerial);

    char userInput[BUFFER_SIZE];
    DWORD bytesWritten;

    while (true) {
        std::cout << "Enter data to send (or 'exit' to quit): ";
        std::cin.getline(userInput, BUFFER_SIZE);

        if (strcmp(userInput, "exit") == 0) {
            shouldExit = true;
            break;
        }

        if (!WriteFile(hSerial, userInput, strlen(userInput), &bytesWritten, nullptr)) {
            std::cerr << "Error writing to serial port." << std::endl;
        }
    }

    // Wait for the serial thread to finish
    serialThread.join();

    CloseHandle(hSerial);
    return 0;
}
