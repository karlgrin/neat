clear
<<<<<<< HEAD

gcc pmhelper.c node.c pib.c cib.c pm.c -Wall -ggdb -o test -ljansson -luv

./test
=======
gcc pmhelper.c node.c pib.c pm.c -Wall -o test.out -ljansson -luv
./test.out
rm test.out
>>>>>>> ffbdff2d7d2c0c73f0125cc32f76f5c09413fb09
