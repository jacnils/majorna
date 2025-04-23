#!/bin/sh
VERSION="$(grep -Po '(?<=VERSION )\d+\.\d+\.\d+' CMakeLists.txt)"
FILES=$(git ls-files | sed "s@/.*@@g" | uniq | grep -vE ".git")

[ -d "majorna" ] && rm -rf majorna/
[ -e "majorna" ] && printf "%s\n" "What???" && exit 1

./scripts/generate_man.sh

mkdir -p majorna
cp -r $FILES majorna/

sha256sum $(find majorna/ -type f) > majorna/sha256
sha512sum $(find majorna/ -type f) > majorna/sha512

tar -cf majorna-$VERSION.tar majorna
gzip majorna-$VERSION.tar
