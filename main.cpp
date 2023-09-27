#include <Windows.h>
#include <iostream>
#include <string>

const char* COM_PORT = "COM8";  // Change to the desired COM port
const int BAUD_RATE = 115200;
const int BUFFER_SIZE = 256;

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

    char buffer[BUFFER_SIZE];
    DWORD bytesRead, bytesWritten;

    while (true) {
        // Read data from the serial port
        if (ReadFile(hSerial, buffer, BUFFER_SIZE, &bytesRead, nullptr)) {
            if (bytesRead > 0) {
                // Print received data to the console
                std::cout.write(buffer, bytesRead);
            }
        }

        // Read data from the user and send it to the serial port
        std::string userInput;
        std::cout << "Enter data to send (or 'exit' to quit): ";
        std::getline(std::cin, userInput);

        if (userInput == "exit") {
            break;
        }

        if (!WriteFile(hSerial, userInput.c_str(), userInput.length(), &bytesWritten, nullptr)) {
            std::cerr << "Error writing to serial port." << std::endl;
        }
    }

    CloseHandle(hSerial);
    return 0;
}
