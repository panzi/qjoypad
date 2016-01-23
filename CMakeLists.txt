cmake_minimum_required(VERSION 2.8.11)

project(qjoypad)

set(QJOYPAD_MAJOR 4)
set(QJOYPAD_MINOR 2)
set(QJOYPAD_PATCH 1)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5X11Extras REQUIRED)

option(WITH_LIBUDEV "Use libudev for automatically updating joypad devices." ON)

if(WITH_LIBUDEV)
	find_package(PkgConfig REQUIRED)

	pkg_check_modules(LIBUDEV libudev)

	if(NOT(LIBUDEV_FOUND))
		message(FATAL_ERROR "libudev not found. If you don't want to compile with libudev support use -DWITH_LIBUDEV=OFF")
	endif()

	link_directories(${LIBUDEV_LIBRARY_DIRS})
	include_directories(${LIBUDEV_INCLUDE_DIRS})
endif()

set(DEVICE_DIR "/dev/input" CACHE PATH "Set the path where QJoyPad will look for your joystick devices. If your devices are /dev/js0, /dev/js1, etc., this should be just \"/dev\". By default, this is /dev/input.")

option(PLAIN_KEYS "Force QJoyPad to use standard XWindows keynames without filtering them for appearance. This will make displays less attractive and readable, but will save processor power and ensure that you see the right names for keys you press." OFF)

option(UPDATE_TRANSLATIONS "Update source translation locale/*.ts
files (WARNING: make clean will delete the source .ts files! Danger!)")

message(STATUS "Using device directory: ${DEVICE_DIR}")

if(PLAIN_KEYS)
	message(STATUS "Using regular XWindows key names.")
	add_definitions(-DPLAIN_KEYS)
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -Wno-deprecated-declarations -Wno-variadic-macros")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

# for config.h
include_directories("${PROJECT_BINARY_DIR}/src")

file(GLOB qjoypad_TRANS_SOURCES translations/qjoypad*.ts)

if(UPDATE_TRANSLATIONS)
	file(GLOB_RECURSE qjoypad_TRANS_FILES *.cpp *.h)
        qt5_create_translation(qjoypad_TRANS ${qjoypad_TRANS_SOURCES} ${qjoypad_TRANS_FILES})
else()
        qt5_add_translation(qjoypad_TRANS ${qjoypad_TRANS_SOURCES})
endif()

add_subdirectory(icons)
add_subdirectory(src)

add_custom_target(translations_target DEPENDS ${qjoypad_TRANS})
add_dependencies(qjoypad translations_target)

install(PROGRAMS qjoypad.desktop DESTINATION "share/applications")
install(FILES ${qjoypad_TRANS} DESTINATION "share/qjoypad/translations")

# uninstall target
configure_file(
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)

add_custom_target(uninstall
	COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")
