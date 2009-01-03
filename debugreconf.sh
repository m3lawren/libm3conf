#!/bin/bash

autoreconf -f -i && \
CFLAGS="-DDEBUG -Wall -Wextra -Werror -g" ./configure --prefix=`pwd`/fakeroot && \
make clean
