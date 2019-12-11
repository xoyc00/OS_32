#!/bin/sh
./img.sh

rm -rf vbox.vdi
VBoxManage convertfromraw image.img vbox.vdi --format VDI --uuid 72d45862-2ca5-481f-84e1-9fc7a93f18f4
