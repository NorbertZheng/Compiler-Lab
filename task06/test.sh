TEST_FILE="result.out"

# check
echo "Compare process begin!"
for i in {1..8}
do
cd test/testsuite$i
python3 ../../tool/comparefile.py ./$TEST_FILE ../bak/testsuite$i/$TEST_FILE
cd ..
cd ..
done
echo "Compare process done!"
