include_directories(${CMAKE_SOURCE_DIR}/include)

set(EXEPRECLIENT
	pressureClient.cc
	pressureClientMain.cc
)

add_executable(PREClient ${EXEPRECLIENT})
target_link_libraries(PREClient forwarding-server )