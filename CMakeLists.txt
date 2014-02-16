cmake_minimum_required(VERSION 2.8.8)

project(qjoypad)

set(QJOYPAD_MAJOR 4)
set(QJOYPAD_MINOR 1)
set(QJOYPAD_PATCH 0)

find_package(Qt4 REQUIRED)

set(DEVICE_DIR "/dev/input" CACHE PATH "Set the path where QJoyPad will look for your joystick devices. If your devices are /dev/js0, /dev/js1, etc., this should be just \"/dev\". By default, this is /dev/input.")

set(PLAIN_KEYS OFF CACHE BOOL "Force QJoyPad to use standard XWindows keynames without filtering them for appearance. This will make displays less attractive and readable, but will save processor power and ensure that you see the right names for keys you press.")

message("-- Using device directory: ${DEVICE_DIR}")

if(PLAIN_KEYS)
	message("-- Using regular XWindows key names.")
	add_definitions(-DPLAIN_KEYS)
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -Wno-deprecated-declarations -Wno-variadic-macros")

	if(NOT(CMAKE_BUILD_TYPE STREQUAL "Debug"))
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
	endif()
endif()

if(NOT(CMAKE_BUILD_TYPE STREQUAL "Debug"))
	add_definitions(-D_DEBUG)
endif()

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

# for config.h
include_directories("${PROJECT_BINARY_DIR}/src")

add_subdirectory(icons)
add_subdirectory(src)

install(PROGRAMS qjoypad.desktop DESTINATION "share/applications")

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)

add_custom_target(uninstall
	COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")
