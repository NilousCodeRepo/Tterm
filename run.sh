#/bin/bash
clear

CC='cc -Wall -Wextra -std=c23 -o Tterm'
COMPILE_FLAGS='-O0 -g3'
LINK_FLAGS='-lX11'

COMPILE_OPT_FLAGS='-O3 -g0'
LINK_OPT_FLAGS='-flto'

GREEN='\033[0;32m'
YELLOW='\033[1;33m'

if [[ -z $1 ]];
	then
		echo -e "${GREEN}DEBUG READY"
		$CC Tterm.c $COMPILE_FLAGS $LINK_FLAGS
	else
		echo -e "${YELLOW}OPTIMIZED"
		$CC Tterm.c $COMPILE_OPT_FLAGS $LINK_OPT_FLAGS
fi
