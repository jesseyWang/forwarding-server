include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCE
	tcpWrap.cc
)

add_library(forwarding-server ${SOURCE})
target_link_libraries(forwarding-server pthread)