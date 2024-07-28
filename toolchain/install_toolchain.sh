#!/bin/bash

BINUTILS_URL=https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.gz
GCC_URL=https://ftp.gnu.org/gnu/gcc/gcc-13.3.0/gcc-13.3.0.tar.gz
BINUTILS_TAR=binutils.tar.gz
GCC_TAR=gcc.tar.gz


TARGET=x86_64-elf

EXTRA_MAKE_FLAGS="-j 8"

PROGRAM_PREFIX=smith-

PWD=$(pwd)
PREFIX=$PWD/cross

MAC=0
if [[ "$(uname)" == "Darwin" ]]; then
    # gcc needs help because macos library paths are a shitshow, see below (ctrl+f MAC) for where patch is applied to change paths
    MAC=1
    echo "Running on MACOS"
    #also switched to newer gcc version, see: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=111632
fi

CONFIRM_BUILD=1

echo using toolchain location:
echo $PREFIX


clean_tc(){
    echo CLEANING TOOLCHAIN DIR
    #delete everything but cross 
    find . -path ./cross -prune -o ! \( -name 'install_toolchain.sh' -o -name 'README.md' \) -exec rm -rf {} + 2> /dev/null
    exit 0
}
usage(){
    echo "Options: "
    echo "-g (GCC_URL)"
    echo "-b (BINUTILS_URL)"
    echo "-c = Clean toolchain dir of all source files and build stuff"
    echo "-u = clean toolchain dir of all built files in cross/*"
    echo "-d = only download"
    echo "-y = dont confirm before building"
}


download_and_extract_sources(){
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
}


make_folders(){
    mkdir cross 2> /dev/null
    mkdir cross/bin 2> /dev/null
    echo "making builddirs"
    mkdir build-binutils 2> /dev/null
    mkdir build-gcc 2> /dev/null
}

while getopts ":ycudb:g:" opt; do
    case $opt in
        y)
            CONFIRM_BUILD=0
            ;;
        c)
            clean_tc
            ;;
        u)
            echo CLEANING ./CROSS will require recompilation of toolchain
            echo "press enter to confirm"
            read -r
            rm -rf cross/*
            ;;
        g)
            GCC_URL=$OPTARG
            ;;
        b)
            BINUTILS_URL=$OPTARG
            ;;
        g)
            download_and_extract_sources
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

download_and_extract_sources

make_folders





if [ -n "$(find ./cross/bin -maxdepth 1 -type f -name '*objdump' )" ]; then
    echo "Bin Utils Already Built."
   
else
    cd build-binutils
    echo Building BinUtils with options:

    echo --target=$TARGET --prefix="$PREFIX" --program-prefix=$PROGRAM_PREFIX --with-sysroot --disable-nls -disable-werror 
    echo and makeflags 
    echo make $EXTRA_MAKE_FLAGS
    echo in dir 
    echo `pwd`
    echo =====
    if [ "$CONFIRM_BUILD" -eq 1 ]; then
        echo "press enter to confirm"
        read -r
    fi
    


    ../binutils*/configure --target=$TARGET --prefix="$PREFIX" --program-prefix="$PROGRAM_PREFIX" --with-sysroot --disable-nls -disable-werror 
    make $EXTRA_MAKE_FLAGS
    make install
    cd ..
fi

echo Building GCC 
echo adding new binutils to path
export PATH="$PATH:$PREFIX/bin"

if [ -n "$(find ./cross/bin/ -maxdepth 2 -type f -name '*gcc' )" ]; then
    echo "GCC Already Built."
   
else
    cd build-gcc
    echo Building GCC with options:

    GCCCMD="../gcc*/configure --target=$TARGET --prefix=\"$PREFIX\" --program-prefix=$PROGRAM_PREFIX --disable-nls --disable-werror --enable-languages=c,c++ --without-headers"
    if [ "$MAC" -eq 1 ]; then
        GCCCMD="$GCCCMD --with-gmp=/opt/homebrew/Cellar/gmp/6.3.0 --with-mpfr=/opt/homebrew/Cellar/mpfr/4.2.1 --with-mpc=/opt/homebrew/Cellar/libmpc/1.3.1"
    fi


    echo "$GCCCMD"
    echo and makeflags 
    echo make $EXTRA_MAKE_FLAGS
    echo in dir 
    echo `pwd`
    echo =====
    if [ "$CONFIRM_BUILD" -eq 1 ]; then
        echo "press enter to confirm"
        read -r
    fi

    eval $GCCCMD
    
    make $EXTRA_MAKE_FLAGS all-gcc 
    make $EXTRA_MAKE_FLAGS all-target-libgcc 
    make install-gcc
    make install-target-libgcc

fi

echo Toolchain Created
echo Your patience has been appreciated