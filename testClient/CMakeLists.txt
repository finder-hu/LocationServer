ADD_EXECUTABLE(client main.cpp TcpClient.cpp DBConnpool.cpp)
TARGET_LINK_LIBRARIES(client mysqlcppconn json)
