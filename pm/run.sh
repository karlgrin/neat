clear
gcc parse_json.c pib.c cib.c node.c pmhelper.c rest.c pm.c -o test -Wall -g -lulfius -ljansson -lpthread
./test
rm test
