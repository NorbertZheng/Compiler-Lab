make
make clean
for i in {1..9}
do
rm -rf testsuite/ex$i.d/reg2dfa.bak/
mkdir testsuite/ex$i.d/reg2dfa.bak/
./reg2dfa.bak < testsuite/ex$i.d/ex$i
mv *.gv testsuite/ex$i.d/reg2dfa.bak/
cd testsuite/ex$i.d/reg2dfa.bak/
dot -Tpdf -o ex$i.ast.pdf ast.gv
dot -Tpdf -o ex$i.nfa.pdf nfa.gv
dot -Tpdf -o ex$i.dfa.pdf dfa.gv
dot -Tpdf -o ex$i.mdfa.pdf mdfa.gv
cd ..
cd ..
cd ..
rm -rf testsuite/ex$i.d/reg2dfa/
mkdir testsuite/ex$i.d/reg2dfa/
./reg2dfa < testsuite/ex$i.d/ex$i
mv *.gv testsuite/ex$i.d/reg2dfa/
cd testsuite/ex$i.d/reg2dfa/
dot -Tpdf -o ex$i.ast.pdf ast.gv
dot -Tpdf -o ex$i.nfa.pdf nfa.gv
dot -Tpdf -o ex$i.dfa.pdf dfa.gv
dot -Tpdf -o ex$i.mdfa.pdf mdfa.gv
cd ..
cd ..
cd ..
python3.4 tool/comparefile.py testsuite/ex$i.d/reg2dfa/ast.gv testsuite/ex$i.d/reg2dfa.bak/ast.gv
python3.4 tool/comparefile.py testsuite/ex$i.d/reg2dfa/nfa.gv testsuite/ex$i.d/reg2dfa.bak/nfa.gv
python3.4 tool/comparefile.py testsuite/ex$i.d/reg2dfa/dfa.gv testsuite/ex$i.d/reg2dfa.bak/dfa.gv
python3.4 tool/comparefile.py testsuite/ex$i.d/reg2dfa/mdfa.gv testsuite/ex$i.d/reg2dfa.bak/mdfa.gv
done

