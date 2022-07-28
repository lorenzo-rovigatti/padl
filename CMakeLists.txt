cmake_minimum_required(VERSION 3.10)

# set the project name
project(padl)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories( ${PROJECT_SOURCE_DIR}/extern/asio ${PROJECT_SOURCE_DIR}/extern )

# add the executables
add_executable(server src/server.cpp src/strings.cpp)
add_executable(client src/client.cpp src/strings.cpp extern/RS-232/rs232.c)

# this is probably not cross platform, to be updated to work on windows
target_link_libraries(server PUBLIC pthread)
target_link_libraries(client PUBLIC pthread)
