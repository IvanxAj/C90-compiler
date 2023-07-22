if which dos2unix ; then
    DOS2UNIX="dos2unix"
fi

echo "========================================"
echo " Cleaning the temporaries and outputs"
make clean

make token_test -B
if [[ "$?" -ne 0 ]]; then
    echo "Build failed.";
fi

mkdir -p test/out
echo "========================================="

for i in test/in/*.txt; do
    echo "==========================="
    echo ""
    echo "Input file : ${i}"
    BASENAME=$(basename $i .txt);
    # cat $i | ${DOS2UNIX} | ./token_test
    cat $i | ${DOS2UNIX} | ./token_test  > test/out/$BASENAME.stdout.txt  2> test/out/$BASENAME.stderr.txt

done
