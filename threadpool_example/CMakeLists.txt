cmake_minimum_required(VERSION 3.14)

project(threadpool_test LANGUAGES CXX)
set(PROJECT_NAME threadpool_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-std=c++17 -pthread -g -Wall -O0 -fstack-protector -fstack-protector-all)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../logger)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/threadpool)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

file(GLOB THREADPOOL_DIR_SRC threadpool/*.cpp threadpool/*.h)
source_group("ThreadPool" FILES ${THREADPOOL_DIR_SRC})


add_executable(${PROJECT_NAME}
	${DICOM_DIR_SRC}
    ../logger/rslogger_declare.h
    ../logger/rslogging.h
    ../logger/rslog.h
    ${THREADPOOL_DIR_SRC}
    ajob.h
    ajob.cpp
    bjob.h
    bjob.cpp
    cjob.h
    cjob.cpp
	main.cpp
)

target_link_libraries(${PROJECT_NAME} rslogger pthread)
