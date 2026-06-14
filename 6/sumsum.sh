#!/bin/sh
# sum() adds up all of its arguments using expr.
#  * returns the sum when every argument is a number;
#  * returns 0 if anything goes wrong (e.g. a non-number argument).
# All error messages from expr are redirected to /dev/null.
#
# Important detail about expr's exit status:
#   0 -> result is a non-zero, non-empty value
#   1 -> result is 0 or empty (NOT an error!)
#   2+ -> an actual error (syntax / non-numeric operand)
# So we only treat status >= 2 as an error, otherwise a legitimate
# sum of 0 would be mistaken for a failure.

sum() {
    total=0
    for arg in "$@"; do
        total=$(expr "$total" + "$arg" 2>/dev/null)
        if [ "$?" -ge 2 ]; then
            echo 0
            return
        fi
    done
    echo "$total"
}

# Read two lines of numbers.
read line1
read line2

# Word-splitting is intentional here so each number becomes an argument.
sum1=$(sum $line1)
sum2=$(sum $line2)

if [ "$sum1" -eq "$sum2" ]; then
    echo "Equal"
else
    echo "Not equal"
fi
