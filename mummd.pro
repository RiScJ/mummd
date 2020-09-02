TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
	inc/

HEADERS += \
	inc/article.hpp

SOURCES += \
		src/main.cpp \
		src/article.cpp
