########################################
#                                      #
#    QMake project file for QJoyPad    #
#                                      #
########################################






#####   Setup Targets   #####

icons.path = $$INSTALL_PREFIX/share/pixmaps/qjoypad
icons.conf_path = $$PREFIX/share/pixmaps/qjoypad
icons.extra = cp ../icons/* $${icons.path}; cd $${icons.path}; ln -sf gamepad4-24x24.png icon24.png; ln -sf gamepad3-64x64.png icon64.png; chmod -R a+r $${icons.path}

doc.path = $$INSTALL_PREFIX/share/doc/qjoypad4
doc.extra = cp ../README.txt ../LICENSE.txt $${doc.path}
target.path = $$INSTALL_PREFIX/bin





#####   Setup Compile   #####

DEFINES += DEVDIR='\\\"$$DEVDIR\\\"'
DEFINES += ICON24='\\\"$${icons.conf_path}/icon24.png\\\"'
DEFINES += ICON64='\\\"$${icons.conf_path}/icon64.png\\\"'

TEMPLATE = app
INCLUDEPATH += .
QMAKE_LIBS += -lXtst -lX11

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
           getkey.h \
           layout_edit.h \
	   quickset.h
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
           main.cpp \
	   quickset.cpp \
	   getkey.cpp


#####      Install      #####

INSTALLS += target icons doc
