#!/bin/bash

cc="clang++ -Wall -Wextra -Werror -W -std=c++98 -g3 -fsanitize=address"
test_ok=0

$cc *.cpp tests/subject_main.cpp > /dev/null 2> /dev/null
./a.out | cat -e > my_output.txt
diff my_output.txt tests/subject_output.txt > /dev/null 2> /dev/null
if [ $? -ne 0 ]
then
    test_ok=1
    echo -e "tests/subject_main.cpp ❌"
    echo -e "\t\033[2m## output ##"
    cat my_output.txt
    echo -ne "\033[0m"
fi
rm my_output.txt

$cc *.cpp tests/compile.cpp > /dev/null 2> /dev/null
if [ $? -ne 0 ]
then
    test_ok=1
    echo -e "tests/compile.cpp ❌"
fi
rm a.out

if [ $test_ok -eq 0 ]
then
    echo -e "cpp_module_02 [\033[32mOK\033[0m]"
else
    echo -e "\ncpp_module_02 [\033[31mKO\033[0m]"
fi