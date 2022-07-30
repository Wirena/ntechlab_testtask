#!/bin/bash
LIBS_DIR="third_party"

function print-error() {
  echo -e "${RED}$1${NORMAL}"
}

function print-green() {
  echo -e "${GREEN}$1${NORMAL}"
}

REPO_ROOT_DIR=$(pwd)

function install-boost() {
  BOOST_URL="https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.gz"
  BOOST_DIR="boost"

  print-green "Downloading Boost Library"
  if [[ -d "$LIBS_DIR/$BOOST_DIR" ]]; then
    rm -rf $LIBS_DIR/$BOOST_DIR/*
  else
    mkdir -p "$LIBS_DIR/$BOOST_DIR"
  fi
  if wget --no-check-certificate "$BOOST_URL" -O - | tar -xz --strip-components 1 -C "$LIBS_DIR/$BOOST_DIR"; then
    print-green "Boost library headers installed"
  else
    print-error "Failed to download and untar Boost\nAbort."
    exit 1
  fi
}

function install-libjpeg() {
  LIBJPEG_URL="http://www.ijg.org/files/jpegsrc.v9d.tar.gz"
  LIBJPEG_BUILD_DIR="libjpeg-build"
  LIBJPEG_LIB_DIR="libjpeg"
  HEADER_PATH=
  STATIC_LIBRARY_PATH="$LIBS_DIR/$LIBJPEG_BUILD_DIR/.libs/libjpeg.a"

  print-green "Donwloading libjpeg Library"

  if [[ -d "$LIBS_DIR/$LIBJPEG_BUILD_DIR" ]]; then
    rm -rf $LIBS_DIR/$LIBJPEG_BUILD_DIR/*
  else
    mkdir -p "$LIBS_DIR/$LIBJPEG_BUILD_DIR"
  fi

  if wget --no-check-certificate "$LIBJPEG_URL" -O - | tar -xz --strip-components 1 -C "$LIBS_DIR/$LIBJPEG_BUILD_DIR"; then
    print-green "Downloaded libjpeg library"
  else
    print-error "Failed to download and untar libjpeg\nAbort."
    exit 1
  fi

  print-green "Building libjpeg static library"
  cd "$LIBS_DIR/$LIBJPEG_BUILD_DIR" || {
    print-error "Failed to cd into build directory: $LIBS_DIR/$LIBJPEG_BUILD_DIR\nAbort."
    exit 1
  }

  ./configure --disable-shared --enable-silent-rules || {
    print-error "Failed to configure libjpeg\nAbort."
    exit 1
  }
  if make -s; then
    print-green "Successfully built libjpeg"
  else
    print-error "Failed to build libjpeg\nAbort."
    exit 1
  fi

  cd $REPO_ROOT_DIR

  print-green "Installing libjpeg headers and static library into repository"

  if [[ -d "$LIBS_DIR/$LIBJPEG_LIB_DIR" ]]; then
    rm -rf $LIBS_DIR/$LIBJPEG_LIB_DIR/*
  else
    mkdir -p "$LIBS_DIR/$LIBJPEG_LIB_DIR"
  fi

  if [[ -d "$LIBS_DIR/$LIBJPEG_LIB_DIR/include" ]]; then
    rm -rf $LIBS_DIR/$LIBJPEG_LIB_DIR/include/*
  else
    mkdir -p "$LIBS_DIR/$LIBJPEG_LIB_DIR/include"
  fi

  cp $LIBS_DIR/$LIBJPEG_BUILD_DIR/*.h $LIBS_DIR/$LIBJPEG_LIB_DIR/include || {
    print-error "Failed to copy headers\nAbort"
    exit 1
  }
  cp "$STATIC_LIBRARY_PATH" $LIBS_DIR/$LIBJPEG_LIB_DIR || {
    print-error "Failed to copy library\nAbort"
    exit 1
  }

  print-green "Header and library installed"
  print-green "Removing build folder"

  if rm -rf "$LIBS_DIR/$LIBJPEG_BUILD_DIR"; then
    print-green "Build folder removed"
  else
    print-error "Build folder not removed"
  fi

  print-green "Libjpeg installed"
}

function print-help() {
  echo -e "Usage:\n-b to install boost\n-j to install libjpeg\nPass no arguments to install both"
}

if [[ $# -eq 0 ]]; then
  install-boost
  install-libjpeg
  exit 0
fi

while [[ $# -gt 0 ]]; do
  case "$1" in
  "-b") install-boost ;;
  "-j") install-libjpeg ;;
  *) print-help ;;
  esac
  shift
done
