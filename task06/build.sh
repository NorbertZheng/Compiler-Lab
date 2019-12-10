# clean
make clean
make clpdf
rm ./test/*.aux ./test/*.log ./test/*.out ./test/*.pdf ./test/expr.tex 
make
# generate bak testsuite result
for i in {1..8}
do
./lambda.bak < ./test/testsuite$i/testsuite > result.out
mv ./expr.tex ./test/bak/testsuite$i/expr.tex
mv ./gmon.out ./test/bak/testsuite$i/gmon.out
mv ./result.out ./test/bak/testsuite$i/result.out
cd test/bak/testsuite$i
pdflatex ./exptree.tex
cd ..
cd ..
cd ..
done
# generate my testsuite result
for i in {1..8}
do
./lambda < ./test/testsuite$i/testsuite > result.out
mv ./expr.tex ./test/testsuite$i/expr.tex
mv ./gmon.out ./test/testsuite$i/gmon.out
mv ./result.out ./test/testsuite$i/result.out
cd test/testsuite$i
pdflatex ./exptree.tex
cd ..
cd ..
done
