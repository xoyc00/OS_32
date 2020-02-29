#!/bin/sh
if echo "$1" | grep -Eq 'i[[:digit:]]86-'; then
  echo i386
else
  echo x86_64
fi
