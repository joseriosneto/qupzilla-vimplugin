# Not using qtLibraryTarget here because it adds suffixes to
# libraries which breaks the tests in mac os x.
TARGET = VimPlugin

HEADERS += include/VimPlugin.h \
           include/VimEngine.h

SOURCES += src/VimPlugin.cpp \
           src/VimEngine.cpp

RESOURCES += vimplugin.qrc

INCLUDEPATH += $$PWD/include/

PLUGIN_DIR = $$PWD
srcdir = $$(QUPZILLA_SRCDIR)
equals(srcdir, "") {
    include(../../plugins.pri)
}
else {
    include($$srcdir/src/plugins.pri)
}
