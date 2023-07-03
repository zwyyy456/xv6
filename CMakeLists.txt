cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_C_STANDARD 17)
project(kern VERSION 0.1.0)

include(CTest)
enable_testing()

add_definitions(-DLAB_PGTBL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}) # 添加当前目录作为头文件搜索路径

aux_source_directory(user src_user)
aux_source_directory(kernel src_kernel)
# 将这些源文件编译成一个函数

add_executable(${PROJECT_NAME} ${src_user} ${src_kernel})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)