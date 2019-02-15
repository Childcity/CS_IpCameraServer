#cmake_minimum_required(VERSION 3.10)
cmake_minimum_required(VERSION 2.6)
project(CS_IpCameraServer)

include_directories(include)
link_directories(${CMAKE_SOURCE_DIR}/libs)
link_directories(${CMAKE_SOURCE_DIR}/libs/boost)
find_package(glog REQUIRED)
set(USED_LIBS boost_system boost_regex boost_thread glog::glog)

# Set global property (all targets are impacted)
#set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
# Set property for my_target only
#set_property(TARGET my_target PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

#set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and
#set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
#set(Boost_USE_MULTITHREADED      ON)

set(CMAKE_CXX_FLAGS "-pthread -std=c++14 -Wall -Wno-reorder")

set(SOURCES
        main.cpp CConfig.cpp CServer.cpp CClientSession.cpp CSQLiteDB.cpp CBusinessLogic.cpp CJsonParser.cpp
        include/sqlite3/sqlite3.c
        include/INIReaderWriter/ini.c
        include/INIReaderWriter/INIReader.cpp
        include/INIReaderWriter/INIWriter.hpp)

set(HEADERS
        main.h CConfig.h CServer.h CClientSession.h CSQLiteDB.h CBusinessLogic.h CJsonParser.h SIpCameraEvent.hpp
        include/sqlite3/sqlite3.h
        include/INIReaderWriter/ini.h
        include/INIReaderWriter/INIReader.h)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries (${PROJECT_NAME} ${USED_LIBS} ${CMAKE_DL_LIBS})