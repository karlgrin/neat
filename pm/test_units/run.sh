clear

gcc ../pmhelper.c ../property.c ../multi_property.c property_test.c all_test.c multi_property_test.c ./../../../Unity/src/unity.c -Wall -o all_test.out -ljansson
 
./all_test.out