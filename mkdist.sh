#!/bin/sh
git submodule init
git submodule update
. ./IDMETA
set -x
git log >ChangeLog
git archive --format=tar --prefix=$NAME-$VERSION/ HEAD > $NAME-$VERSION.tar
tar xf $NAME-$VERSION.tar
cp ChangeLog $NAME-$VERSION
cp -r doc/common $NAME-$VERSION/doc
cd $NAME-$VERSION
rm -fr debian
cd doc
make
cd ../..
tar cfz $NAME-$VERSION.tar.gz $NAME-$VERSION
rm $NAME-$VERSION.tar
rm -r $NAME-$VERSION
exit 0


