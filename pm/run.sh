clear

gcc pmhelper.c node.c pib.c -Wall -o test.out -ljansson

valgrind ./test.out

rm test.out