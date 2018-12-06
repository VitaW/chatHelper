cmake_minimum_required(VERSION 3.5)
project(mini)

set(CMAKE_CXX_STANDARD 11)

link_libraries(boost_system)

link_libraries(boost_log)

link_libraries(boost_thread)

link_libraries(pthread)
link_libraries(boost_log_setup)

link_libraries(mysqlclient)

link_libraries(jsoncpp)

add_executable(mini Log.h DBOperation.cpp MySql.cpp reply.cpp reply.hpp Log.h request_parser.cpp request_parser.hpp
        main.cpp MiniServer.cpp MiniServer.h Buffer.cpp TcpConnection.cpp TcpConnection.h Manager.cpp Manager.h)
