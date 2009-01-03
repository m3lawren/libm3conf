#!/bin/bash

autoreconf -f -i && \
CFLAGS="-Wall -Wextra -Werror -g" ./configure --prefix=`pwd`/fakeroot && \
make clean
