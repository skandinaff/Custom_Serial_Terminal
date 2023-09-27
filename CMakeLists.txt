cmake_minimum_required(VERSION 3.0)
project(SerialPortTerminal)

add_executable(SerialPortTerminal main.cpp)

# Add Windows-specific flags and libraries for serial port communication
if(WIN32)
    target_compile_definitions(SerialPortTerminal PRIVATE -DWIN32_LEAN_AND_MEAN)
    target_link_libraries(SerialPortTerminal PRIVATE setupapi)
endif()
