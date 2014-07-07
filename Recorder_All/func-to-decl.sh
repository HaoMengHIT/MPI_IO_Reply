#!/bin/sh

sed -e 's:\(\w\+\) \(\w\+\)\([^;]\+\):RECORDER_FORWARD_DECL(P\2,\1,\3):' 
