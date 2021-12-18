QT += core widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp

include($$PWD/occ/OccPart.pri)
include($$PWD/robotics/RobotPart.pri)
include($$PWD/Tools/Tools.pri)
include($$PWD/Gui/Gui.pri)

DESTDIR = $$PWD/bin

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
