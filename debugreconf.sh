#!/bin/bash

autoreconf -f -i && \
CFLAGS="-DM3CONF_DEBUG -Wall -Wextra -Werror -g" ./configure --prefix=`pwd`/fakeroot && \
make clean
