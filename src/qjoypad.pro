########################################
#                                      #
#    QMake project file for QJoyPad    #
#                                      #
########################################
#                                      #
#   Edit these values if you need to:  #
#                                      #
########################################

	# Where QJoyPad will look for joystick devices:
	DEFINES += DEVDIR='"/dev/input"'
	
	# Where QJoyPad will be installed:
	target.path = /usr/local/bin
	
########################################
#                                      #
#    Leave everything else unchanged.  #
#                                      #
########################################








TEMPLATE = app
DEPENDPATH += trayicon
INCLUDEPATH += . trayicon
QMAKE_LIBS += -lXtst

# Input
HEADERS += axis.h \
           axis_edit.h \
           axisw.h \
           button.h \
		   button_edit.h \
           buttonw.h \
           constant.h \
           device.h \
           error.h \
           event.h \
           flash.h \
           icon.h \
           joypad.h \
           joypadw.h \
		   joyslider.h \
           keycode.h \
           layout.h \
           layout_edit.h \
           loop.h \
		   quickset.h \
           trayicon/trayicon.h
SOURCES += axis.cpp \
           axis_edit.cpp \
           axisw.cpp \
           button.cpp \
		   button_edit.cpp \
           buttonw.cpp \
           event.cpp \
           flash.cpp \
           joypad.cpp \
           joypadw.cpp \
		   joyslider.cpp \
           keycode.cpp \
           layout.cpp \
           layout_edit.cpp \
           loop.cpp \
           main.cpp \
		   quickset.cpp \
           trayicon/trayicon.cpp \
           trayicon/trayicon_x11.cpp

INSTALLS += target
