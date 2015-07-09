project(telegram-n9)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0003 NEW)

# Dependencies ===============================================================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt4 REQUIRED QtCore QtGui)
find_package(OpenSSL REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(tgl/ tgl/build/)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/tgl/build/libs)

# Sources ====================================================================
# .ui files are listed so that QtCreator will list them in the project tree
set(src_pairs TelegramWindow)
set(telegram_n9_UIS TelegramWindow.ui)
set(telegram_n9_HEADERS $<JOIN:${src_pairs},.h >)
set(telegram_n9_SOURCES ${telegram_n9_HEADERS} ${telegram_n9_UIS}
    $<JOIN:${src_pairs},.cpp >
    main.cpp
    qttimer.cpp
    qtconnection.cpp
)

# Create output ==============================================================
# Declare the target (an executable)
add_executable(${PROJECT_NAME} ${telegram_n9_SOURCES})

target_link_libraries(${PROJECT_NAME} tgl event 
    ${OPENSSL_LIBRARIES} readline Qt4::QtCore Qt4::QtGui)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
