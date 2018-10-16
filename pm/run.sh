clear
gcc pmhelper.c property.c multi_property.c node.c pib.c -Wall -o test.out -ljansson
./test.out
rm test.out