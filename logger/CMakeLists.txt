cmake_minimum_required(VERSION 3.14)

project(rslogger LANGUAGES CXX)

set(PROJECT_NAME rslogger)
set(PROJECT_NAME_STATITC rslogger_static)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "current platform: Linux ")
    SET(LINUX 1)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "current platform: Windows")
    SET(OS_WIN 1)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    MESSAGE(STATUS "current platform: FreeBSD")
ELSE ()
    MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

add_definitions(-DCLASS_EXPORTS)
add_definitions(-DFUNC_EXPORTS)



#link_directories(${PROJECT_SOURCE_DIR}/../build)


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
message("The curr src dir:${CMAKE_CURRENT_SOURCE_DIR}")

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR})

set(logger_src 
	rslogger_declare.h
	comparable.h

	curthread.h
	curthread.cpp
	floatingbuffer.h
	floatingbuffer.cpp
	fmt.h
	fmt.cpp
	logstream.h
	logstream.cpp
	rslog.h
	rslog.cpp
	rslogging.h
	rslogging.cpp
	stringpiece.h
	stringpiece.cpp
	timestamp.h
	timestamp.cpp
	)
if (OS_WIN)
	set(logger_src_win  
                win/dirent.h
                win/dirent.cpp)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)	
endif()

if (OS_WIN)
    add_library(${PROJECT_NAME_STATITC} STATIC ${logger_src} ${logger_src_win})
else()
    add_library(${PROJECT_NAME_STATITC} STATIC ${logger_src})
endif()
# 将 rspromethusipc_static 重命名为 rspromethusipc
# set_target_properties(${PROJECT_NAME_STATITC} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
# cmake 在构建一个新的 target 时，会尝试清理掉使用这个名称的库，
# 所以在构建 libadd.so 时，就会清理掉 libadd.a
set_target_properties(${PROJECT_NAME_STATITC} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
if (OS_WIN)
target_link_libraries(${PROJECT_NAME_STATITC} ws2_32)
endif()
#link_libraries(ws2_32)
if (OS_WIN)
    add_library(${PROJECT_NAME} SHARED ${logger_src} ${logger_src_win})
else()
    add_library(${PROJECT_NAME} SHARED ${logger_src})
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
if (OS_WIN)
target_link_libraries(${PROJECT_NAME} ws2_32)
endif()

