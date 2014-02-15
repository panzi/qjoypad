set(qjoypad_SOURCES 
	axis.cpp
	axis_edit.cpp
	axisw.cpp
	button.cpp
	button_edit.cpp
	buttonw.cpp
	event.cpp
	flash.cpp
	getkey.cpp
	icon.cpp
	joypad.cpp
	joypadw.cpp
	joyslider.cpp
	keycode.cpp
	layout.cpp
	layout_edit.cpp
	main.cpp
	quickset.cpp)

set(qjoypad_HEADERS
	axis_edit.h
	axis.h
	axisw.h
	button_edit.h
	button.h
	buttonw.h
	constant.h
	device.h
	error.h
	event.h
	flash.h
	getkey.h
	icon.h
	joypad.h
	joypadw.h
	joyslider.h
	keycode.h
	layout_edit.h
	layout.h
	quickset.h)

QT4_WRAP_CPP(qjoypad_HEADERS_MOC ${qjoypad_HEADERS})
add_executable(qjoypad ${qjoypad_SOURCES} ${qjoypad_HEADERS_MOC})
target_link_libraries(qjoypad ${QT_LIBRARIES} Xtst X11)

install(TARGETS qjoypad RUNTIME DESTINATION "bin")
