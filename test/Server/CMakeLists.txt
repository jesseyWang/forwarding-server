include_directories(${CMAKE_SOURCE_DIR}/include)

set(EXESERVER
	serverMain.cc
	server.cc
)

add_executable(Server ${EXESERVER})
target_link_libraries(Server forwarding-server )
