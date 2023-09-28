cmake_minimum_required(VERSION 3.0)
project(SerialPortTerminal)

# Find the Qt5 package
set(CMAKE_PREFIX_PATH "C:/Qt63/5.15.2/msvc2019/bin")
find_package(Qt5Widgets REQUIRED)

# Set the C++ standard to C++11 or higher
set(CMAKE_CXX_STANDARD 11)

# Add the source files (main.cpp)
add_executable(SerialPortTerminal main.cpp)

# Link Qt5 libraries to your project
target_link_libraries(SerialPortTerminal PRIVATE Qt5::Widgets)

# Add Windows-specific flags and libraries for serial port communication
if(WIN32)
    target_compile_definitions(SerialPortTerminal PRIVATE -DWIN32_LEAN_AND_MEAN)
    target_link_libraries(SerialPortTerminal PRIVATE setupapi)
endif()
