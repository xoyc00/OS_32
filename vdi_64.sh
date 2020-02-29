#!/bin/sh
./img_64.sh

rm -rf vbox.vdi
VBoxManage convertfromraw image64.img vbox.vdi --format VDI --uuid 6ca52d9f-05b8-4995-acdf-42c5ae6cbf33
