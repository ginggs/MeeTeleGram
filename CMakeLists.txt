project(telegram-n9)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0003 NEW)

# Dependencies ===============================================================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt4 REQUIRED QtCore)
find_package(OpenSSL REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(tgl/ tgl/build/)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/tgl/build/libs)

# Sources ====================================================================
# .ui files are listed so that QtCreator will list them in the project tree
set(telegram_n9_UIS )
set(telegram_n9_HEADERS telegram.h loop.h interface.h)
set(telegram_n9_SOURCES ${telegram_n9_HEADERS} ${telegram_n9_UIS}
    main.c
    loop.c
    interface.c
)

# Create output ==============================================================
# Declare the target (an executable)
add_executable(${PROJECT_NAME} ${telegram_n9_SOURCES})

target_link_libraries(${PROJECT_NAME} tgl event 
    ${OPENSSL_LIBRARIES} readline Qt4::QtCore)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
