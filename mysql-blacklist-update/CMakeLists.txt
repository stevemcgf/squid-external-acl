add_executable (mysql-blacklist-update src/main.cpp src/blacklist/MySqlWriter.cpp src/blacklist/BlacklistReader.cpp)

target_link_libraries(mysql-blacklist-update mariadbclient pthread z m ssl crypto dl)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -O3" )
