clear

gcc ../pmhelper.c ../property.c ../multi_property.c ../node.c pmhelper_test.c property_test.c multi_property_test.c node_test.c test_constant.c test_all.c ./../../../Unity/src/unity.c -Wall -o test_all.out -ljansson
 
./test_all.out

rm test_all.out