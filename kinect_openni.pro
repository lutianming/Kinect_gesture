# -------------------------------------------------
# Project created by QtCreator 2011-12-21T23:26:32
# -------------------------------------------------
TARGET = kinect_openni
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    opennicontroler.cpp \
    kinectreader.cpp \
    GestureRecognizer.cpp
HEADERS += mainwindow.h \
    opennicontroler.h \
    kinectreader.h \
    GestureRecognizer.h
FORMS += mainwindow.ui
INCLUDEPATH += /usr/include/ni
LIBS += /usr/lib/libOpenNI.so
