icarus/t/milestones/
====================

This directory of tests is special as each test in here is considered a
"milestone" in the development of Icarus.

Each milestone has 2 parts:

 - `xx.ic` - input icarus source code
 - `xx.exp` - the expected stdout from running the matching `xx.ic` file

At any given time, many of the milestones in here will not succeed.

The `progress` file sets the expectations for each milestone,
the first column is the milestone name,
the second column is the expected result for the `2c` backend,
the third column is the expected result for the `pancake` backend.

This progress files supports many options, see `test_milestones.pl` comments
to see what each of these options means.

