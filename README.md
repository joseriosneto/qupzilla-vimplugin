# Vim Plugin for QupZilla
How awesome is [Vimium](https://github.com/philc/vimium)? This plugin try to port its functionalities to [QupZilla](https://github.com/QupZilla/qupzilla). Have in mind the project is still on its early stages. 

# Compiling

The easiest way as [other plugins](https://github.com/QupZilla/qupzilla-plugins) is to copy the plugin directory into QupZilla's source tree (eg. into src/plugins).

For building outside QupZilla's tree you need to export the `QUPZILLA_SRCDIR` environment variable with QupZilla's source directory. For instance, if QupZilla was cloned in `~/workspace/` in Linux:

    $ export QUPZILLA_SRCDIR=$HOME/workspace/qupzilla/

Then you can compile running these commands:

    $ qmake
    $ make
    $ sudo make install

# Keyboard Bindings

Navigating the current page:

    h       scroll left
    j       scroll down
    k       scroll up
    l       scroll right
    gg      scroll to top of the page
    G       scroll to bottom of the page
    d       scroll down half a page
    u       scroll up half a page
    r       reload page
    
Tabs:

    J       previous tab
    K       next tab
    x       close current tab
    X       open last closed tab
