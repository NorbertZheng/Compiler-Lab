# clean
make clean
make clpdf
rm ./test/*.aux ./test/*.log ./test/*.out ./test/*.pdf ./test/expr.tex 
make
./lambda < ./test/testsuite
mv ./expr.tex ./test/
mv ./gmon.out ./test/
cd test
pdflatex ./exptree.tex
python3 ../tool/comparefile.py ./exptree.tex ./bak/exptree.tex

