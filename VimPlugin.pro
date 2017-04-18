TARGET = $$qtLibraryTarget(VimPlugin)
# OS/2 allows only 8 chars in TARGET
os2: TARGET = VimPlugi

# We need QupZilla source
qupzilla_src_dir = $$(QUPZILLA_SRCDIR)
equals(qupzilla_src_dir, "") {
    error("QUPZILLA_SRCDIR env var not defined!")
}

# We need libQupZilla. OSX specially since 'make install'
# does not install it in default directory
qupzilla_lib_dir = $$(QUPZILLA_LIBDIR)
equals(qupzilla_lib_dir, "") {
    mac {
        error("QUPZILLA_LIBDIR env var must be defined for mac osx!")
    }
    qupzilla_lib_dir = /usr/lib/
}

QT += webenginewidgets

TEMPLATE = lib
CONFIG += plugin
DESTDIR = $$qupzilla_src_dir/bin/plugins/

OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build

HEADERS += include/VimPlugin.h \
           include/VimEngine.h

SOURCES += src/VimPlugin.cpp \
           src/VimEngine.cpp

RESOURCES += vimplugin.qrc

INCLUDEPATH += $$PWD/include/                           \
               $$qupzilla_src_dir/src/lib/adblock       \
               $$qupzilla_src_dir/src/lib/app           \
               $$qupzilla_src_dir/src/lib/autofill      \
               $$qupzilla_src_dir/src/lib/bookmarks     \
               $$qupzilla_src_dir/src/lib/cookies       \
               $$qupzilla_src_dir/src/lib/downloads     \
               $$qupzilla_src_dir/src/lib/history       \
               $$qupzilla_src_dir/src/lib/navigation    \
               $$qupzilla_src_dir/src/lib/network       \
               $$qupzilla_src_dir/src/lib/notifications \
               $$qupzilla_src_dir/src/lib/opensearch    \
               $$qupzilla_src_dir/src/lib/other         \
               $$qupzilla_src_dir/src/lib/plugins       \
               $$qupzilla_src_dir/src/lib/popupwindow   \
               $$qupzilla_src_dir/src/lib/preferences   \
               $$qupzilla_src_dir/src/lib/rss           \
               $$qupzilla_src_dir/src/lib/session       \
               $$qupzilla_src_dir/src/lib/sidebar       \
               $$qupzilla_src_dir/src/lib/tabwidget     \
               $$qupzilla_src_dir/src/lib/tools         \
               $$qupzilla_src_dir/src/lib/webengine     \
               $$qupzilla_src_dir/src/lib/webtab        \
               $$qupzilla_src_dir/src/lib/3rdparty      \

DEPENDPATH += $$INCLUDEPATH                     \
              $$qupzilla_src_dir/src/lib/data   \

LIBS += -L$$qupzilla_lib_dir -lQupZilla

!mac:unix {
    target.path = $$qupzilla_lib_dir/qupzilla/
    INSTALLS += target
}
