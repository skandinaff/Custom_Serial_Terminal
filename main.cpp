#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QThread>
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>

const char* COM_PORT = "COM8";  // Change to the desired COM port
const int BAUD_RATE = 115200;
const int BUFFER_SIZE = 256;

std::atomic<bool> shouldExit(false);

void ReadSerialData(HANDLE hSerial, QTextEdit* textArea) {
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;

    while (!shouldExit) {
        if (ReadFile(hSerial, buffer, BUFFER_SIZE, &bytesRead, nullptr)) {
            if (bytesRead > 0) {
                // Print received data to the GUI's text area
                QString receivedData = QString::fromLatin1(buffer, bytesRead);
                QMetaObject::invokeMethod(textArea, "append", Qt::QueuedConnection, Q_ARG(QString, receivedData));

                // Print received data to the console
                std::cout.write(buffer, bytesRead);
                std::cout.flush();
            }
        }
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Serial Port Terminal");

    QVBoxLayout* layout = new QVBoxLayout(&window);

    QTextEdit* textArea = new QTextEdit(&window);
    layout->addWidget(textArea);

    QLineEdit* inputField = new QLineEdit(&window);
    layout->addWidget(inputField);

    QPushButton* sendButton = new QPushButton("Send", &window);
    layout->addWidget(sendButton);

    // Create a thread for asynchronous serial data reading
    HANDLE hSerial = CreateFileA(COM_PORT, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

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

    std::thread serialThread(ReadSerialData, hSerial, textArea);

    QObject::connect(sendButton, &QPushButton::clicked, [&]() {
        if (serialThread.joinable()) {
            if (!WriteFile(hSerial, inputField->text().toLatin1().data(), inputField->text().toLatin1().size(), nullptr, nullptr)) {
                std::cerr << "Error writing to serial port." << std::endl;
            }
        }
    });

    QObject::connect(&window, &QWidget::destroyed, [&]() {
        shouldExit = true;
        if (serialThread.joinable()) {
            serialThread.join();
        }
        CloseHandle(hSerial);
    });

    window.show();

    return app.exec();
}
