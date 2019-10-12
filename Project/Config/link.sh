#!/bin/bash
echo "create link path:$1"
ln -s `pwd`/core $1/core
ln -s `pwd`/envir $1/envir
