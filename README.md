# Vim Plugin for QupZilla
How awesome is Vimium? This plugin try to port its functionalities to [QupZilla](https://github.com/QupZilla/qupzilla). Have in mind the project is still on its early stages. 

# Compiling
You need QupZilla source to compile the code. Export the `QUPZILLA_SRCDIR` envinronment variable with QupZilla's source directory. For instance, if QupZilla was cloned in `~/workspace/` in Linux:

    $ export QUPZILLA_SRCDIR=$HOME/workspace/qupzilla/src/

Then you can compile the plugin running these commands:

    $ qmake
    $ make
    $ sudo make install
