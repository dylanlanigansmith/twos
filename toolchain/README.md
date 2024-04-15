### Builds a cross compiler w/ BinUtils and GCC 
```


To install run 
./install_toolchain.sh 



be warned it builds gcc which takes a while
and I have make hardcoded running with -j 14 (change EXTRA_MAKE_FLAGS in .sh)

it also assumes you have all the dependencies to build gcc
if you have cloned this repo and somehow haven't wound up with those installed already in the past, color me impresssed

also haven't tested it besides on archlinux x86/64


```