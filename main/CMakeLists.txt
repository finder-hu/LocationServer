ADD_EXECUTABLE(server main.cpp Guide.cpp Location.cpp Database.cpp ConnPool.cpp MsgQueue.cpp Point.cpp TcpServer.cpp TcpUser.cpp Util.cpp)
INCLUDE_DIRECTORIES(../include)
TARGET_LINK_LIBRARIES(server json event mysqlcppconn pthread)
