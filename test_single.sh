#!/bin/bash

set -uo pipefail

# Clean bin directory
echo "Cleaning bin directory..."
rm -rf ./bin/*
mkdir -p bin/output

# Compile the project
make bin/c_compiler

# Initialize variables
TOTAL=0
PASSING=0
J_UNIT_OUTPUT_FILE="./bin/junit_results.xml"
printf '%s\n' '<?xml version="1.0" encoding="UTF-8"?>' > "${J_UNIT_OUTPUT_FILE}"
printf '%s\n' '<testsuite name="Integration test">' >> "${J_UNIT_OUTPUT_FILE}"

fail_testcase() {
    echo -e "\t> ${1}"
    printf '%s\n' "<error type=\"error\" message=\"${1}\">${1}</error>" >> "${J_UNIT_OUTPUT_FILE}"
    printf '%s\n' "</testcase>" >> "${J_UNIT_OUTPUT_FILE}"
}

test_file=$1 # Take the test name as a command-line argument

# Remove any trailing .c if present
test_file="${test_file%.c}"

TO_ASSEMBLE="custom_tests/${test_file}.c"
LOG_PATH="./bin/output/${test_file}"
OUT="${LOG_PATH}"
DRIVER="custom_tests/${test_file}_driver.c"

echo "${TO_ASSEMBLE}"
printf '%s\n' "<testcase name=\"${TO_ASSEMBLE}\">" >> "${J_UNIT_OUTPUT_FILE}"

OUT="${LOG_PATH}"
rm -f "${OUT}.s"
rm -f "${OUT}.o"
rm -f "${OUT}"
./bin/c_compiler -S "${TO_ASSEMBLE}" -o "${OUT}.s" 2> "${LOG_PATH}.compiler.stderr.log" > "${LOG_PATH}.compiler.stdout.log"

if [ $? -ne 0 ]; then
    fail_testcase "Fail: see ${LOG_PATH}.compiler.stderr.log and ${LOG_PATH}.compiler.stdout.log"
    exit 1
fi

riscv64-unknown-elf-gcc -march=rv32imfd -mabi=ilp32d -o "${OUT}.o" -c "${OUT}.s" 2> "${LOG_PATH}.assembler.stderr.log" > "${LOG_PATH}.assembler.stdout.log"
if [ $? -ne 0 ]; then
    fail_testcase "Fail: see ${LOG_PATH}.assembler.stderr.log and ${LOG_PATH}.assembler.stdout.log"
    exit 1
fi

riscv64-unknown-elf-gcc -march=rv32imfd -mabi=ilp32d -static -o "${OUT}" "${OUT}.o" "${DRIVER}" 2> "${LOG_PATH}.linker.stderr.log" > "${LOG_PATH}.linker.stdout.log"
if [ $? -ne 0 ]; then
    fail_testcase "Fail: see ${LOG_PATH}.linker.stderr.log and ${LOG_PATH}.linker.stdout.log"
    exit 1
fi

spike pk "${OUT}" > "${LOG_PATH}.simulation.log"
if [ $? -eq 0 ]; then
    echo -e "\t> Pass"
    printf '%s\n' "</testcase>" >> "${J_UNIT_OUTPUT_FILE}"
else
    fail_testcase "Fail: simulation did not exit with exit-code 0"
    exit 1
fi

printf '%s\n' '</testsuite>' >> "${J_UNIT_OUTPUT_FILE}"
