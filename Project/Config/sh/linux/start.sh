#!/bin/bash
./NetGrid --name=master --type=0 --id=1 --port=7001 &
./NetGrid --name=gate --type=1 --id=1 --port=8101 &
./NetGrid --name=relation --type=3 --id=1 --port=8301 &
