cmake_minimum_required(VERSION "3.20")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

project("my-project" VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)
find_package(Tesseract REQUIRED)
find_package(Leptonica REQUIRED)
message(STATUS "OpenCV found: ${OpenCV_FOUND}")
message(STATUS "Tesseract found: ${Tesseract_FOUND}")
message(STATUS "Leptonica found: ${Leptonica_FOUND}")
message(STATUS "Tesseract include dir: ${Tesseract_INCLUDE_DIRS}")
message(STATUS "Tesseract_LIBRARIES: ${Tesseract_LIBRARIES}")
message(STATUS "Tesseract_LIBRARY_DIRS: ${Tesseract_LIBRARY_DIRS}")

link_directories(${Tesseract_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} "src/main.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE ${Tesseract_INCLUDE_DIRS} ${Leptonica_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} Tesseract::libtesseract ${Leptonica_LIBRARIES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/tessdata
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/eng.traineddata" $<TARGET_FILE_DIR:${PROJECT_NAME}>/tessdata/eng.traineddata)


find_package(GTest REQUIRED)

#if(NOT GTest_INCLUDE_DIRS OR NOT GTest_LIBRARIES)
#    message(WARNING "GTest paths are missing, manually setting them.")
#    set(GTest_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/build/x64-Debug/vcpkg_installed/x64-windows/include")
#    set(GTest_LIBRARIES "${CMAKE_SOURCE_DIR}/build/x64-Debug/vcpkg_installed/x64-windows/lib/gtest.lib")
#endif()

#message(STATUS "GTest include dir: ${GTest_INCLUDE_DIRS}")
#message(STATUS "GTest libraries: ${GTest_LIBRARIES}")

enable_testing()

add_executable(${PROJECT_NAME}_tests "tests/test_main.cpp")
target_link_libraries(${PROJECT_NAME}_tests
  PRIVATE
    GTest::gtest
    GTest::gtest_main
)
#target_include_directories(${PROJECT_NAME}_tests PRIVATE ${GTest_INCLUDE_DIRS})
#target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${GTest_LIBRARIES})
add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)