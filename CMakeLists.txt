cmake_minimum_required(VERSION 3.25)
project(p2p)

set(CMAKE_CXX_STANDARD 17)

# Enable automatic processing of Qt files
set(CMAKE_AUTOMOC ON) # For .cpp files with Q_OBJECT macros
set(CMAKE_AUTORCC ON) # For .qrc resource files
set(CMAKE_AUTOUIC ON) # For .ui files

# Locate Qt6 Widgets
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# Locate Boost
find_package(Boost REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost library not found!")
endif()

# Locate OpenSSL
find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL library not found!")
endif()

# Locate SQLite
find_package(SQLite3 REQUIRED)
if(NOT SQLite3_FOUND)
    message(FATAL_ERROR "SQLite3 library not found!")
endif()

# Locate LibtorrentRasterbar
find_package(LibtorrentRasterbar REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
        src/main.cpp
        src/networking/peer.cpp
        src/networking/dht.cpp
        src/networking/nat.cpp
        src/messaging/message.cpp
        src/ui/mainwindow.cpp
        src/ui/ui.cpp
        src/networking/stun.cpp
        src/networking/dht_manager.cpp
        )

# UI files are automatically handled by AUTOUIC
set(HEADERS
        include/ui/mainwindow.h include/utils.h)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        SQLite::SQLite3
        Qt6::Widgets
        Qt6::Network
        LibtorrentRasterbar::torrent-rasterbar
        )

# Ensure UIC-generated headers are included
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})