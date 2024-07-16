TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        Mesh.cpp \
        BMFFileLoader.cpp \
        NodalFields.cpp \
        AbstractResultsFileLoader.cpp \
        SMRFileLoader.cpp \
        main.cpp

HEADERS += \
    Mesh.h \
    BMFFileLoader.h \
    NodalFields.h \
    AbstractResultsFileLoader.h \
    SMRFileLoader.h

QMAKE_LFLAGS = -fopenmp
QMAKE_CXXFLAGS += -fopenmp
