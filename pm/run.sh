clear
gcc pmhelper.c node.c pib.c cib.c pm.c -Wall -ggdb -o test -ljansson -luv
./test
rm test
