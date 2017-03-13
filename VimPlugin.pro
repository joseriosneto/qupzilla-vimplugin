TARGET = $$qtLibraryTarget(VimPlugin)
# OS/2 allows only 8 chars in TARGET
os2: TARGET = VimPlugi

# We need QupZilla source
qupzilla_src_dir = $$(QUPZILLA_SRCDIR)
equals(qupzilla_src_dir, "") {
    error("QUPZILLA_SRCDIR env var not defined!")
}

QT += webenginewidgets

TEMPLATE = lib
CONFIG += plugin
DESTDIR = $$qupzilla_src_dir/../bin/plugins/

OBJECTS_DIR = build
MOC_DIR = build
RCC_DIR = build

HEADERS += include/VimPlugin.h

SOURCES += src/VimPlugin.cpp

RESOURCES += vimplugin.qrc

INCLUDEPATH += $$PWD/include/                       \
               $$qupzilla_src_dir/lib/adblock \
               $$qupzilla_src_dir/lib/app \
               $$qupzilla_src_dir/lib/autofill \
               $$qupzilla_src_dir/lib/bookmarks \
               $$qupzilla_src_dir/lib/cookies \
               $$qupzilla_src_dir/lib/downloads \
               $$qupzilla_src_dir/lib/history \
               $$qupzilla_src_dir/lib/navigation \
               $$qupzilla_src_dir/lib/network \
               $$qupzilla_src_dir/lib/notifications \
               $$qupzilla_src_dir/lib/opensearch \
               $$qupzilla_src_dir/lib/other \
               $$qupzilla_src_dir/lib/plugins \
               $$qupzilla_src_dir/lib/popupwindow \
               $$qupzilla_src_dir/lib/preferences \
               $$qupzilla_src_dir/lib/rss \
               $$qupzilla_src_dir/lib/session \
               $$qupzilla_src_dir/lib/sidebar \
               $$qupzilla_src_dir/lib/tabwidget \
               $$qupzilla_src_dir/lib/tools \
               $$qupzilla_src_dir/lib/webengine         \
               $$qupzilla_src_dir/lib/webtab \
               $$qupzilla_src_dir/lib/3rdparty \

DEPENDPATH += $$INCLUDEPATH               \
              $$qupzilla_src_dir/lib/data \

LIBS += -lQupZilla

!mac:unix {
    target.path = /usr/lib/qupzilla
    INSTALLS += target
}
