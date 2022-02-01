#-------------------------------------------------
#
# Project created by QtCreator 2020-03-10T21:32:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MulticlassClassifier
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += open_cv/include

LIBS += $${_PRO_FILE_PWD_}/open_cv/bin/libopencv_calib3d320.dll
LIBS += $${_PRO_FILE_PWD_}/open_cv/bin/libopencv_core320.dll
LIBS += $${_PRO_FILE_PWD_}/open_cv/bin/libopencv_features2d320.dll
LIBS += $${_PRO_FILE_PWD_}/open_cv/bin/libopencv_highgui320.dll
LIBS += $${_PRO_FILE_PWD_}/open_cv/bin/libopencv_imgcodecs320.dll
LIBS += $${_PRO_FILE_PWD_}/open_cv/bin/libopencv_photo320.dll
LIBS += $${_PRO_FILE_PWD_}/open_cv/bin/libopencv_imgproc320.dll
LIBS += $${_PRO_FILE_PWD_}/open_cv/bin/libopencv_ml320.dll

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    csvreader.cpp \
    stringlistmodel.cpp \
    datacollector.cpp \
    labelshash.cpp \
    classifier.cpp \
    normalizer.cpp \
    resultmatrix.cpp

HEADERS += \
        mainwindow.h \
    csvreader.h \
    stringlistmodel.h \
    datacollector.h \
    labelshash.h \
    classifier.h \
    normalizer.h \
    resultmatrix.h

FORMS += \
        mainwindow.ui
