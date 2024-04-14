#!/bin/bash

BINUTILS_URL=https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.gz
GCC_URL=https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz

clean(){
    find . ! \( -name 'install_toolchain.sh' -o -name 'README.md' \) -type f -exec rm -f {} +
    
}


while getopts ":b:g:" opt; do
    case $opt in
        clean)
            BINUTILS_URL=$OPTARG
            ;;
        )
            GCC_URL=$OPTARG
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

echo Installing Toolchain

wget  -O binutils.tar.gz $BINUTILS_URL
wget  -O gcc.tar.gz $GCC_URL

tar -xvzf binutils.tar.gz 
tar -xvzf gcc.tar.gz

rm *.tar.gz