clear
gcc pmhelper.c node.c pib.c pm.c -Wall -o test.out -ljansson -luv
./test.out
rm test.out