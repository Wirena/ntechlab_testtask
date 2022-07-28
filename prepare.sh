#!/bin/bash
BOOST_URL="https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.gz"
LIBS_DIR="third_party"
BOOST_DIR_NAME="boost"

function print-error() {
  echo "${RED}$1${NORMAL}"
}

function print-green() {
  echo "${GREEN}$1${NORMAL}"
}

function download-boost() {
  if [[ -d "$LIBS_DIR/$BOOST_DIR_NAME" ]]; then
    rm -rf $LIBS_DIR/$BOOST_DIR_NAME/*
  else
    mkdir -p "$LIBS_DIR/$BOOST_DIR_NAME"
  fi
  wget --no-check-certificate "$BOOST_URL" -O - | tar -xz --strip-components 1 -C "$LIBS_DIR/$BOOST_DIR_NAME"
}

download-boost
