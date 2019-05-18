#! /bin/bash
			
for f in $(find . -name '*.c' -or -name '*.cpp' -or -name '*.h' -type f)
do
    astyle --style=ansi -s4 -k3 -jSfpHUcnC $f
done

