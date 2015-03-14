Quick notes on how to use gcov

See http://linux.die.net/man/1/gcov

Gcov is automatically linked in via and make command (see config.mk)

To use gcov simply:

* run any binary
* run gcov over the source file you are interested in
* inspect the generation file

Example:

    make clean
    make test
    ./bin/test_pvector
    gvoc src/pvector


Running gcov output this:

    chris@heimdall:~/devel/icarus$ gcov src/pvector.c
    File 'src/pvector.c'
    Lines executed:63.33% of 60
    Creating 'pvector.c.gcov'

Which tells us the file we need to inspect:

    vim pvector.c.gcov

Which then shows each line and the gcov stats for it.


