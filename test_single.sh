set -e

# Check that the script is called with exactly one argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path-to-c-file>"
    exit 1
fi

# Make sure the compiler binary exists
make bin/c_compiler

C_FILE_PATH="$1"

BASE_NAME=$(basename "$C_FILE_PATH" .c)

# Create the output directory if it doesn't exist
mkdir -p bin/output
rm -rf bin/output/*

# Path for the output assembly file and logs
ASM_FILE_PATH="bin/output/${BASE_NAME}.s"
COMPILER_STDOUT_LOG="bin/output/${BASE_NAME}.compiler.stdout.log"
COMPILER_STDERR_LOG="bin/output/${BASE_NAME}.compiler.stderr.log"

./bin/c_compiler -S "$C_FILE_PATH" -o "$ASM_FILE_PATH" 2> "$COMPILER_STDERR_LOG" > "$COMPILER_STDOUT_LOG"

echo "Compiled $C_FILE_PATH to $ASM_FILE_PATH"
