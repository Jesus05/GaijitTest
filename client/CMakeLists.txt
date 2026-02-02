cmake_minimum_required(VERSION 3.16)

project(GaijinTestClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(GaijinTestClient main.cpp)

target_link_libraries(GaijinTestClient PRIVATE ws2_32 wsock32)

target_compile_definitions(GaijinTestClient PRIVATE ASIO_STANDALONE)
target_include_directories(GaijinTestClient PRIVATE ../third/asio-1.36.0/include)

include(GNUInstallDirs)
install(TARGETS GaijinTestClient
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)