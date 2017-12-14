#!/bin/bash

cd output/system
tar czf /nfs/_tmp.tar usr
mv /nfs/_tmp.tar /nfs/_binstosync.tar
echo "bins prepared"

