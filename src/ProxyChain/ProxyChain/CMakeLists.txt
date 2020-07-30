# CMakeList.txt : CMake project for Socks5Client, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(project_name ProxyChain)
project(${project_name})

# Add source to this project's executable.
add_executable (${project_name} "ProxyChain.cpp")

if(MSVC)
set(MSVC_LIBS ws2_32)
else(MSVC)
set(MSVC_LIBS)
endif(MSVC)

target_link_libraries(${project_name}
PRIVATE ${MSVC_LIBS}
)
# TODO: Add tests and install targets if needed.
