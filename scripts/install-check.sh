#!/bin/bash
# Install check in ./vendor/

GIT_REF="0.15.2"
DEST="vendor/"

mkdir -p $DEST
cd $DEST

if [[ ! -d check ]]; then
  git clone https://github.com/libcheck/check.git
fi

cd check
git checkout "$GIT_REF"

autoreconf --install &&
  ./configure --prefix=$PWD/.. &&
  make &&
  make install