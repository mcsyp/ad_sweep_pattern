QT += core network
QT -= gui

CONFIG += c++11

TARGET = pattern_classifier
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../QriNeuronKit/QriNeuronLib
INCLUDEPATH += ../pattern_trainer

LIBS += -L../../QriNeuronKit/build-QriNeuronLib-Desktop_Qt_5_8_0_clang_64bit-Debug -lQriNeuronLib

SOURCES += main.cpp \
    ../pattern_trainer/csvsamples.cpp \
    ../pattern_trainer/featureclassifier.cpp \
    ../pattern_trainer/framefeatureclassifier.cpp \
    ../pattern_trainer/framefeatureextractor.cpp \
    ../pattern_trainer/tools.cpp \
    configparser.cpp \
    ../pattern_trainer/featurebasic.cpp \
    multipatternservice.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH+=../gesture_trainer/
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    ../pattern_trainer/csvsamples.h \
    ../pattern_trainer/featureclassifier.h \
    ../pattern_trainer/framefeatureclassifier.h \
    ../pattern_trainer/framefeatureextractor.h \
    ../pattern_trainer/tools.h \
    configparser.h \
    ../pattern_trainer/pattern_config.h \
    ../pattern_trainer/featurebasic.h \
    multipatternservice.h

