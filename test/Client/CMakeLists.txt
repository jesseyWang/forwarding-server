include_directories(${CMAKE_SOURCE_DIR}/include)

# set(EXESERVER
# 	serverMain.cc
# 	server.cc
# )

set(EXECLIENT
	clientMain.cc
	client.cc
)

# add_executable(Server ${EXESERVER})
# target_link_libraries(Server forwarding-server )

add_executable(Client ${EXECLIENT})
target_link_libraries(Client forwarding-server )