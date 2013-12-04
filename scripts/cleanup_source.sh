#!/bin/zsh

echo "Note, this is designed to run on a mac with homebrew and might not run elsewhere without tweaking"

echo "clang-format"
ls **/*.cpp **/*.h | parallel -j 4 clang-format-3.4 --style=Google -i

echo "autopep8"
ls **/*.py | parallel -j 4 autopep8 -i --aggressive --aggressive

echo "patch autopep8"
sed -i.old "s/^\# \\\\example/\#\# \\\\example/g" **/*.py

#echo "include-what-you-use"
#ls **/*.cpp | gsort --random-sort | parallel -j 8 "include-what-you-use {} '--check_also=include/RMF/*.h' '--check_also=include/RMF/internal/*.h' --check_also=bin/common.h -I src -I include -I ../debug/include -I ../debug/src -I /usr/local/Cellar/llvm/3.3/lib/clang/3.3/include/ --std=c++11 |& /usr/local/Cellar/include-what-you-use/fix_includes.py"
