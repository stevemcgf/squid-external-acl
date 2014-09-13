add_executable (mysql-blacklist-acl src/main.cpp src/blacklist/MySqlReader.cpp)

target_link_libraries(mysql-blacklist-acl mariadbclient pthread z m ssl crypto dl)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -O3" )
