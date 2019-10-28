make
make clean
rm -rf testsuite/extra-ex.d/reg2dfa.bak/
mkdir testsuite/extra-ex.d/reg2dfa.bak/
./reg2dfa.bak < testsuite/extra-ex.d/rep0_9B.txt
mv *.gv testsuite/extra-ex.d/reg2dfa.bak/
cd testsuite/extra-ex.d/reg2dfa.bak/
dot -Tpdf -o extra-ex.ast.pdf ast.gv
dot -Tpdf -o extra-ex.dfa.pdf dfa.gv
dot -Tpdf -o extra-ex.mdfa.pdf mdfa.gv
cd ..
cd ..
cd ..
rm -rf testsuite/extra-ex.d/reg2dfa/
mkdir testsuite/extra-ex.d/reg2dfa/
./reg2dfa < testsuite/extra-ex.d/rep0_9B.txt
mv *.gv testsuite/extra-ex.d/reg2dfa/
cd testsuite/extra-ex.d/reg2dfa/
dot -Tpdf -o extra-ex.ast.pdf ast.gv
dot -Tpdf -o extra-ex.dfa.pdf dfa.gv
dot -Tpdf -o extra-ex.mdfa.pdf mdfa.gv
cd ..
cd ..
cd ..
python3.4 tool/comparefile.py testsuite/extra-ex.d/reg2dfa/ast.gv testsuite/extra-ex.d/reg2dfa.bak/ast.gv
python3.4 tool/comparefile.py testsuite/extra-ex.d/reg2dfa/dfa.gv testsuite/extra-ex.d/reg2dfa.bak/dfa.gv
python3.4 tool/comparefile.py testsuite/extra-ex.d/reg2dfa/mdfa.gv testsuite/extra-ex.d/reg2dfa.bak/mdfa.gv


