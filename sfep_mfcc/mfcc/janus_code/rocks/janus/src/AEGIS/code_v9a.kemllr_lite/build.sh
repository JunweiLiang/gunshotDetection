#!/bin/sh

gmake
gmake install
pushd .
cd ../..
gmake ibisNX_opt
popd
