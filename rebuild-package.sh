#!/bin/sh

mkdir -p "slackware/usr"

cd ../../src

make clean
./config --prefix="/usr" && make || exit 1

installwatch -o ../dist/$DIST/slackware/temp make install

cd ../dist/$DIST/slackware

rm install/doinst.sh

sed -r -e '/\s(open|symlink)/!d' -e '/\/dev\/tty/d' -e 's/.*\sopen\s\/(.*)\s.*/cp --parents \/\1 ./' -e 's/.*\ssymlink\s\/(.*)\s\/(.*)\s.*/ln -s \/\1 \2/' -i temp


chmod a+x temp
./temp
rm temp

chown -R root:root *
chown -R root:bin usr/bin

/sbin/makepkg --linkadd y --chown n ../$DIST-i486-1.tgz

cd ..
