icarus/t/
========

All tests under here are run as part of `make test`.

`make test` will also run the tests for the libraries included under `../libs/`.

The directories here:

 - `unit` contains c unit tests testing individual units of our code
 - `custom` contains tests which require a "custom" driver (usually a perl script), these serve as integration tests.
 - `milestones` contains a set of tests that represent milestones in Icarus development, see `milestones/README.md` for more information.


