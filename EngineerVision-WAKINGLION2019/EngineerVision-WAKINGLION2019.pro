QT += core
QT -= gui

CONFIG += c++11

TARGET = EngineerVision-WAKINGLION2019
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += -I/usr/local/include \
               -I/usr/local/include/opencv \
               -I/usr/local/include/opencv2 \

LIBS += -L/usr/local/lib -lopencv_ml -lopencv_dnn -lopencv_objdetect -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_photo -lopencv_shape -lopencv_video -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_imgproc -lopencv_flann -lopencv_core

QMAKE_CFLAGS  =`pkg-config --cflags opencv`
LIBS += `pkg-config --libs opencv` -lMVSDK
INCLUDEPATH = -I./include

HEADERS += \
    EngineerVision-WAKINGLION2019/src/ChestDetection.h \
    EngineerVision-WAKINGLION2019/src/ColorDetector.h \
    EngineerVision-WAKINGLION2019/src/Common.h \
    EngineerVision-WAKINGLION2019/src/Gui.h \
    EngineerVision-WAKINGLION2019/src/IndVideoCapture.h \
    EngineerVision-WAKINGLION2019/src/InfantryInfo.h \
    EngineerVision-WAKINGLION2019/src/JudgementInfo.h \
    EngineerVision-WAKINGLION2019/src/Kalman.h \
    EngineerVision-WAKINGLION2019/src/MVVideoCapture.h \
    EngineerVision-WAKINGLION2019/src/PackData.h \
    EngineerVision-WAKINGLION2019/src/PnpSolver.h \
    EngineerVision-WAKINGLION2019/src/Preprocessing.h \
    EngineerVision-WAKINGLION2019/src/Protocol.h \
    EngineerVision-WAKINGLION2019/src/RMVideoCapture.hpp \
    EngineerVision-WAKINGLION2019/src/Serial.h \
    EngineerVision-WAKINGLION2019/src/Settings.h \
    EngineerVision-WAKINGLION2019/src/TargetDetection.h \
    EngineerVision-WAKINGLION2019/src/Tool.h \
    EngineerVision-WAKINGLION2019/src/VideoCaptureFactory.h \
    EngineerVision-WAKINGLION2019/src/VisualPid.h

SOURCES += \
    EngineerVision-WAKINGLION2019/src/ChestDetection.cpp \
    EngineerVision-WAKINGLION2019/src/ColorDetector.cpp \
    EngineerVision-WAKINGLION2019/src/Gui.cpp \
    EngineerVision-WAKINGLION2019/src/IndVideoCapture.cpp \
    EngineerVision-WAKINGLION2019/src/InfantryInfo.cpp \
    EngineerVision-WAKINGLION2019/src/Kalman.cpp \
    EngineerVision-WAKINGLION2019/src/main.cpp \
    EngineerVision-WAKINGLION2019/src/MVVideoCapture.cpp \
    EngineerVision-WAKINGLION2019/src/PackData.cpp \
    EngineerVision-WAKINGLION2019/src/PnpSolver.cpp \
    EngineerVision-WAKINGLION2019/src/Preprocessing.cpp \
    EngineerVision-WAKINGLION2019/src/Protocol.cpp \
    EngineerVision-WAKINGLION2019/src/RMVideoCapture.cpp \
    EngineerVision-WAKINGLION2019/src/Serial.cpp \
    EngineerVision-WAKINGLION2019/src/Settings.cpp \
    EngineerVision-WAKINGLION2019/src/TargetDetection.cpp \
    EngineerVision-WAKINGLION2019/src/Tool.cpp \
    EngineerVision-WAKINGLION2019/src/VideoCaptureFactory.cpp
