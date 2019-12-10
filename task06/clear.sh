flag="clean"

make clean
make clpdf
# clean bak testsuite result
for i in {1..8}
do 
cd test/bak/testsuite$i
make $flag
cd ..
cd ..
cd ..
done
# generate my testsuite result
for i in {1..8}
do 
cd test/testsuite$i
make $flag
cd ..
cd ..
done

