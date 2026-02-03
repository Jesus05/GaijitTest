cmake_minimum_required(VERSION 3.16)

project(GaijinTestServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GaijinTestServer main.cpp
  config.h config.cpp
  values.h
  tcpserver.h tcpserver.cpp
  tcpconnection.h tcpconnection.cpp
  statisticsmanager.h statisticsmanager.cpp
  stats.h)

target_link_libraries(GaijinTestServer PRIVATE ws2_32 wsock32)

target_compile_definitions(GaijinTestServer PRIVATE ASIO_STANDALONE)
target_include_directories(GaijinTestServer PRIVATE ../third/asio-1.36.0/include)

include(GNUInstallDirs)
install(TARGETS GaijinTestServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
