#!/bin/bash

#builds grub on macos so we can easily use grub-mkrescue to make our final image
#ty https://gist.github.com/emkay/a1214c753e8c975d95b4

PWD=$(cd ..; pwd)
MAC=0
if [[ "$(uname)" == "Darwin" ]]; then
    MAC=1
fi

echo "found root dir \"$PWD\" "
echo SETUP THIRDPARTY TOOLS


THIRDPARTYDIR="$PWD/thirdparty"


TC_PREFIX="$PWD/toolchain/cross/bin/smith"
echo "TC_PREFIX = $TC_PREFIX"
TARGET=x86_64

GRUB_INSTALLDIR="build"
PREFIX="$THIRDPARTYDIR/$GRUB_INSTALLDIR"

if [ $MAC -eq 1 ]; then
  if [ ! -e "./$GRUB_INSTALLDIR/bin/grub-mkrescue" ]; then
      echo "Looks like we need to build GRUB, no installation found (searching in '$PREFIX') "
      cd $THIRDPARTYDIR
      if [ ! -d "grub" ]; then
        echo "Downloading Grub Source"
        git clone --depth 1 git://git.savannah.gnu.org/grub.git
      else 
        echo "GRUB Source Already Downloaded"
      fi
      mkdir -p $GRUB_INSTALLDIR
      GRUB_TOOLS="TARGET_CC=$TC_PREFIX-gcc TARGET_OBJCOPY=$TC_PREFIX-objcopy TARGET_STRIP=$TC_PREFIX-strip TARGET_NM=$TC_PREFIX-nm TARGET_RANLIB=$TC_PREFIX-ranlib GRUB_PLATFORMS=\"emu efi pc\" GRUB_TARGET_CPU=x86_64"

    
      cd grub
      export TARGET=$TARGET
      export PREFIX=$PREFIX
      if [ ! -e "$THIRDPARTYDIR/grub/.bootstrapped" ]; then
        echo "Bootstrap script running"
        ./bootstrap
        touch $THIRDPARTYDIR/grub/.bootstrapped
      fi
      mkdir -p build-grub
      cd build-grub
      #I manually had to patch $target_cpu to get efi build to work due to some old script stuff for when grub actually ran on macs
      # it like tries to fix stuff manually if it detects macos (ie. force efi and target)
      CFG_GRUB="../configure --disable-werror --target=$TARGET --prefix=$PREFIX --with-platform=efi $GRUB_TOOLS"
      echo "$CFG_GRUB"
      eval "$CFG_GRUB"
      alias awk="gawk"
      make 
      make install

      echo Built Grub

      cd $THIRDPARTYDIR

      #there is a bug for grub efi not properly setting up fonts so lets just grab manually
      curl -Lo $GRUB_INSTALLDIR/share/grub/unicode.pf2 https://github.com/nlamirault/muk/raw/master/grub/fonts/unicode.pf2

      echo "Installing mtools, xorriso, (deps. of grub_mkrescue)"
      brew install mtools xorriso

  else
    echo "GRUB Already Built  @ $PREFIX"
  fi
else 
  echo "building GRUB should not necessary on Linux-esque platforms"
#if you are using windows trying to build this project just stop.
fi


OVMF_INSTALLDIR="ovmf"
OVMF_FILE=OVMF64.fd

OVMF_PATH="$THIRDPARTYDIR/$OVMF_INSTALLDIR/$OVMF_FILE"

OVMF_URL=https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd

if [ ! -e "$OVMF_PATH" ]; then
  cd $THIRDPARTYDIR
  echo "Downloading and Installing OVMF"
  mkdir -p $OVMF_INSTALLDIR
  cd $OVMF_INSTALLDIR 
  curl -Lo $OVMF_PATH $OVMF_URL
  echo "installed OVMF64.fd @ $OVMF_PATH"

else
  echo "OVMF Firmware already installed @ $OVMF_PATH"
fi

