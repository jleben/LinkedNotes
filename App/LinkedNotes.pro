TEMPLATE = app
HEADERS += Attachments.hpp TagTreeModel.hpp TagTreeWidget.hpp TagBrowser.hpp ItemEditor.hpp MainInterface.hpp
SOURCES += Attachments.cpp TagTreeModel.cpp TagTreeWidget.cpp TagBrowser.cpp ItemEditor.cpp MainInterface.cpp
LIBS += -L../Lib/ -llinkednotes
TARGET = linkednotes
MOC_DIR = tmp
OBJECTS_DIR = tmp