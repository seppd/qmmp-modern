QT += widgets

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050400

SOURCES +=\
    mainwindow.cpp \
    skin.cpp \
    container.cpp \
    group.cpp \
    layout.cpp \
    guiobject.cpp \
    bitmap.cpp \
    layer.cpp \
    button.cpp \
    text.cpp \
    truetypefont.cpp \
    bitmapfont.cpp \
    slider.cpp \
    script.cpp \
    system.cpp \
    togglebutton.cpp \
    list.cpp \
    popupmenu.cpp \
    region.cpp \
    timer.cpp \
    componentbucket.cpp \
    edit.cpp \
    browser.cpp \
    eqvis.cpp \
    status.cpp \
    title.cpp \
    animatedlayer.cpp \
    grouplist.cpp \
    cfggroup.cpp \
    mouseredir.cpp \
    dropdownlist.cpp \
    layoutstatus.cpp \
    tabsheet.cpp \
    variable.cpp \
    interpreter.cpp \
    callback.cpp \
    windowsystem.cpp \
    map.cpp \
    modernfactory.cpp \
    dock.cpp \
    skinreader.cpp \
    windowholder.cpp \
    visualizer.cpp \
    fft.c

HEADERS  += mainwindow.h \
    debug.h \
    skin.h \
    container.h \
    group.h \
    layout.h \
    guiobject.h \
    bitmap.h \
    layer.h \
    button.h \
    text.h \
    truetypefont.h \
    bitmapfont.h \
    slider.h \
    script.h \
    system.h \
    togglebutton.h \
    list.h \
    popupmenu.h \
    region.h \
    timer.h \
    componentbucket.h \
    edit.h \
    browser.h \
    eqvis.h \
    status.h \
    title.h \
    animatedlayer.h \
    grouplist.h \
    cfggroup.h \
    mouseredir.h \
    dropdownlist.h \
    layoutstatus.h \
    tabsheet.h \
    variable.h \
    interpreter.h \
    callback.h \
    windowsystem.h \
    function.h \
    map.h \
    modernfactory.h \
    dock.h \
    skinreader.h \
    bitmapstorage.h \
    windowholder.h \
    visualizer.h \
    fft.h

TARGET = qmmp-modern
TEMPLATE = lib

CONFIG += c++11 plugin

LIBS += -lqmmp \
    -lqmmpui

unix {
    isEmpty(LIB_DIR) {
        LIB_DIR = /lib
    }

    target.path = $$LIB_DIR/qmmp/Ui
    INSTALLS += target
    CONFIG += link_pkgconfig
    PKGCONFIG += x11
    DEFINES += QMMP_WS_X11
    QT += x11extras
}
