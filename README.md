# Vim Plugin for QupZilla
How awesome is [Vimium](https://github.com/philc/vimium)? This plugin try to port its functionalities to [QupZilla](https://github.com/QupZilla/qupzilla). Have in mind the project is still on its early stages. 

# Compiling
You need QupZilla's source to compile the code. Export the `QUPZILLA_SRCDIR` environment variable with QupZilla's source directory. For instance, if QupZilla was cloned in `~/workspace/` in Linux:

    $ export QUPZILLA_SRCDIR=/home/joseriosneto/workspace/qupzilla/bin/

If you have installed `libQupZilla.*` in a custom directory (or if you are using Mac OSX) you also need to specify this path in the `QUPZILLA_LIBDIR` environment variable. For instance, if QupZilla was cloned in `~/workspace/` in Mac OSX:

    $ export QUPZILLA_LIBDIR=/Users/joseriosneto/workspace/qupzilla/bin/

Then you can compile running these commands:

    $ qmake
    $ make
    $ sudo make install
