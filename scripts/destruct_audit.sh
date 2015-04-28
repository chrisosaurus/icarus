#/usr/bin/env bash

# this script will output a line for every _new it can find which does
# not have a matching _destroy

# this script only searches header files

# note that this will have false positives if we mention either _new(s) or _destroy(s)
# in comments

# get all destroys
find . -iname '*.h' | xargs grep -oh 'ic_\w*_destroy(' | sed 's/_destroy(//g' | sort | uniq > DESTROYS

# get all news
find . -iname '*.h' | xargs grep -oh 'ic_\w*_new(' | sed 's/_new(//g' | sort | uniq > NEWS

echo "types that have a _new and that are missing a _destroy"

# find diff
# display every type with a new but not destroy
for each in `cat NEWS`; do
    grep "$each" DESTROYS > /dev/null
    # grep will exit with 0 if found
    #                 and 1 is not
    if [ $? -eq 1 ]; then
        echo $each
    fi
done

rm DESTROYS
rm NEWS

