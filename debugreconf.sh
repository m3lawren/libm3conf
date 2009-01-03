#!/bin/bash

autoreconf -f -i && \
CFLAGS="-DM3CONF_DEBUG -g" ./configure --prefix=`pwd`/fakeroot && \
make clean
