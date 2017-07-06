QT += core network
QT -= gui

CONFIG += c++11

TARGET = gesture_service
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../QriNeuronKit/QriNeuronLib
LIBS += -L../../QriNeuronKit/build-QriNeuronLib-Desktop_Qt_5_8_0_clang_64bit-Debug -lQriNeuronLib

SOURCES += main.cpp \
    ../sweep_feature_trainer/keyaxselectorclassifier.cpp \
    ../sweep_feature_trainer/sweepfeatureclassifier.cpp \
    ../sweep_feature_trainer/sweepfeatureextractor.cpp \
    serviceclient.cpp \
    serviceprotocol.cpp \
    sweepthread.cpp \
    configparser.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    ../sweep_feature_trainer/keyaxselectorclassifier.h \
    ../sweep_feature_trainer/sweepfeatureclassifier.h \
    ../sweep_feature_trainer/sweepfeatureextractor.h \
    serviceclient.h \
    serviceprotocol.h \
    global_service.h \
    sweepthread.h \
    configparser.h

