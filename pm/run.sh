clear
gcc pmhelper.c parse_json.c node.c pib.c cib.c pm.c -Wall -ggdb -o test -ljansson -luv
./test
rm test
