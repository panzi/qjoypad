########################################
#                                      #
#    QMake project file for QJoyPad    #
#                                      #
########################################






#####   Setup Targets   #####

target.path = $$PREFIX/bin

icons.path = $$PREFIX/share/pixmaps/qjoypad
icons.files = icons/*

doc.path = $$PREFIX/doc/qjoypad3
doc.files = README.txt

postinstall.path = $${icons.path}
postinstall.extra = cd $${icons.path}; ln -sf gamepad1-24x24.png icon24.png; ln -sf gamepad1-64x64.png icon64.png






#####   Setup Compile   #####

DEFINES += DEVDIR='"$$DEVDIR"'
DEFINES += ICON24='"$${icons.path}/icon24.png"'
DEFINES += ICON64='"$${icons.path}/icon64.png"'

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
		   icon.cpp \
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





#####      Install      #####

INSTALLS += target icons doc postinstall
