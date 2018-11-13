clear
gcc parse_json.c pib.c node.c pmhelper.c rest.c -o rest -Wall -g -lulfius -ljansson
./rest
rm rest
