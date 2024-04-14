#!/bin/bash

BINUTILS_URL=https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.gz
GCC_URL=https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz
BINUTILS_TAR=binutils.tar.gz
GCC_TAR=gcc.tar.gz


clean_tc(){
    echo CLEANING TOOLCHAIN DIR
    find . ! \( -name 'install_toolchain.sh' -o -name 'README.md' \) -type f -exec rm -f {} +
    exit 0
}
usage(){
    echo "Options: "
    echo "-g (GCC_URL)"
    echo "-b (BINUTILS_URL)"
    echo "-clean = Clean toolchain dir"
}

while getopts ":cb:g:" opt; do
    case $opt in
        c)
            clean_tc
            ;;
        g)
            GCC_URL=$OPTARG
            ;;
        b)
            BINUTILS_URL=$OPTARG
            ;;
        :)
            echo "Option -$OPTARG requires an argument."
            usage
            ;;
        \?)
            echo "Invalid option: -$OPTARG"
            usage
            ;;
    esac
done
if [ $OPTIND -eq 1 ]; then
    echo "No options provided. Installing Toolchain with defaults"
fi


# Check if subdirectories containing gcc* or binutils* exist
if [ -n "$(find . -maxdepth 1 -type d -name 'gcc*' -o -name 'binutils*')" ]; then
    echo "Subdirectories containing gcc and binutils exist."
    # Handle the existence of such subdirectories here
else
    if [ -e "$BINUTILS_TAR" ] && [ -e "$GCC_TAR" ]; then
        echo "archived GCC and binutils files already exist. Skipping download."
    else
        echo "Downloading gcc and binutils source files..."
        wget -O "$BINUTILS_TAR" "$BINUTILS_URL"
        wget -O "$GCC_TAR" "$GCC_URL"
    fi

    echo "Extracting source archives"
    tar -xvzf $BINUTILS_TAR
    tar -xvzf $GCC_TAR
    rm *.tar.gz
fi

echo Building GCC 

PWD=$(pwd)
PREFIX=$PWD/cross
mkdir cross 2> /dev/null
mkdir cross/bin 2> /dev/null

echo using toolchain location:
echo $PREFIX
echo adding to path
export PATH="$PATH:$PREFIX/bin"

echo "making builddirs"
mkdir build-binutils 2> /dev/null
mkdir build-gcc 2> /dev/null

TARGET=x86_64-elf

EXTRA_MAKE_FLAGS=-j 14

PROGRAM-PREFIX=dls-

if [ -n "$(find ./cross/bin -maxdepth 1 -type f -name '*objdump' )" ]; then
    echo "Bin Utils Already Built."
   
else
    echo "Building BinUtils"
    cd build-binutils
    echo Building BinUtils
    ../binutils*/configure --target=$TARGET --prefix="$PREFIX" --program-prefix=$PROGRAM_PREFIX --with-sysroot --disable-nls -disable-werror 
    echo `pwd`
    echo running make
    make $EXTRA_MAKE_FLAGS
    make install

fi

if [ -n "$(find ./cross/bin -maxdepth 1 -type f -name '*gcc' )" ]; then
    echo "GCC Already Built."
   
else
    echo "Building GCC"
    cd build-gcc
    echo Building BinUtils
    ../gcc*/configure --target=$TARGET --prefix="$PREFIX" --program-prefix=$PROGRAM_PREFIX --disable-nls -disable-werror  --enable-languages=c,c++ --without-headers 
    
    make $EXTRA_MAKE_FLAGS all-gcc 
    make $EXTRA_MAKE_FLAGS all-target-libgcc 
    make install-gcc
    make install-target-libgcc

fi

echo Toolchain Created