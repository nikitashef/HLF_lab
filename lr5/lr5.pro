QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    lab5.cpp \
    param.cpp

HEADERS += \
    lab5.h \
    param.h

FORMS += \
    lab5.ui

# Расскоммент для fftw
# LIBS += -L"../../lr4" -lfftw3-3
# INCLUDEPATH += "../../lr4"
# DEFINES += FFTW_AVAILABLE

# For other systems, configure FFTW paths accordingly

qnx: target.path = /tmp/$${TARGET}/bin

!isEmpty(target.path): INSTALLS += target
