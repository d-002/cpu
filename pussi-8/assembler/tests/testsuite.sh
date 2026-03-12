#!/bin/sh

BIN_PATH="$(pwd)/assembli"

# timeout per test
TIMEOUT=1s

BLUE="\033[34m"
GREEN="\033[32m"
ORANGE="\033[33m"
RED="\033[31m"
RESET="\033[37m"
BACK_LINE="\033[1A                                                       \n\033[1A"
SUCCESS=0
ERROR=2

tested=0
passing=0
failing=0

# argument: the directory to use for the test
# the directory should contain:
# - options, a file with the options to pass to the binary
# - return_code, containing the expected return code
check() {
    test_dir=$1
    test_name=$BLUE$(basename "$test_dir" | sed 's/_/ /g')$RESET

    pwd_before=$(pwd)
    cd "$test_dir"
    bin_code=$(timeout $TIMEOUT $BIN_PATH $(cat options) 1>bin_stdout 2>bin_stderr)
    bin_code=$?
    ref_code=$(cat return_code)

    printf "Testing $BLUE$(echo $comment | sed -e 's/^\(.\{45\}\).*$/\1.../')"
    printf "$RESET                        \n"

    ok=true
    if [ $bin_code = 124 ]; then
        printf "${RED}FAILED$RESET $test_name (bin timed out after $TIMEOUT seconds)\n"
        ok=false

        echo "Stdout:"
        cat bin_stdout

        echo -e "\nStderr:"
        cat bin_stderr
    else
        if [ "$bin_code" -ne "$ref_code" ]; then
            printf "${RED}FAILED$RESET $test_name (wrong return code):\n"
            printf "Expected $ref_code, got $RED$bin_code$RESET\n"
            ok=false
        fi
        if [ \( -z "$(cat ref_stdout)" -a -n "$(cat bin_stdout)" \) -o \
            \( -n "$(cat ref_stdout)" -a -z "$(grep -i "$(cat ref_stdout)" bin_stdout)" \) ]; then
            printf "${RED}FAILED$RESET $test_name (wrong output on stdout):\n"
            diff bin_stdout ref_stdout
            ok=false
        fi
        if [ \( -z "$(cat ref_stderr)" -a -n "$(cat bin_stderr)" \) -o \
            \( -n "$(cat ref_stderr)" -a -z "$(grep -i "$(cat ref_stderr)" bin_stderr)" \) ]; then
            printf "${RED}FAILED$RESET $test_name (wrong errput on stderr):\n"
            diff bin_stderr ref_stderr
            ok=false
        fi
    fi

    rm -f bin_stdout bin_stderr
    cd "$pwd_before"

    tested=$((tested + 1))

    if [ $ok = true ]; then
        passing=$((passing + 1))
        printf $BACK_LINE
    else
        failing=$((failing + 1))
        echo
    fi
}

# ===== main testsuite ===== #

for dir in tests/functional/*; do
    check "$dir"
done

# ===== results ===== #

if [ $tested = 0 ]; then
    printf "=================== $GREEN✔ No tests ran$RESET ===================       \n"
    percentage=0
else
    percentage=$(($passing * 100 / $tested))

    if [ $failing = 0 ]; then
        printf "====== $GREEN✔ All tests passed$RESET  "
        col=$GREEN
    elif [ $percentage -gt 80 ]; then
        printf "====== $ORANGE✖ Some tests failed$RESET "
        col=$ORANGE
    else
        printf "====== $RED✖ Some tests failed$RESET "
        col=$RED
    fi

    printf "($col$passing$RESET/$tested passed, $col$failing$RESET failed) =======       \n"
fi

if [ -n "$OUTPUT_FILE" ]; then
    echo "Set up to write percentage to '$OUTPUT_FILE', will write now"
    echo $percentage > "$OUTPUT_FILE"
fi

[ $failing = 0 ] && exit 0 || exit 1
