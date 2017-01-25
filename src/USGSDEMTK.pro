android|ios|winrt {
    error( "This example is not supported for android, ios, or winrt." )
}

SOURCES += main.cpp \
           surfacegraph.cpp \
    Dem.cpp \
    Point3.cpp

HEADERS += surfacegraph.h \
    Dem.h \
    Point3.h

QT += widgets
QT += datavisualization

