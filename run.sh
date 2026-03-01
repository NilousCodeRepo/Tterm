#/bin/bash
clear

FILES='src/Tterm.c src/init.c'
INCLUDE_PATH='include/'

BINARY_NAME='Tterm'
BINARY_NAME_DEBUG='debug_Tterm'

CC="cc -Wall -Wextra -std=c23 -I${INCLUDE_PATH}"
COMPILE_FLAGS='-march=native -O0 -g3 -fverbose-asm -masm=intel'
LINK_FLAGS='-Wl,--start-group -lX11 -Wl,--end-group'

COMPILE_OPT_FLAGS='-march=native -Ofast -g0 -ffast-math -s -masm=intel'
LINK_OPT_FLAGS='-Wl,--start-group -lX11 -Wl,--end-group -flto'

GREEN='\033[0;32m'
YELLOW='\033[1;33m'

if [[ -z $1 ]];
	then
		echo -e "${GREEN}DEBUG READY"
		$CC ${FILES} -o ${BINARY_NAME_DEBUG} $COMPILE_FLAGS $LINK_FLAGS
	else
		echo -e "${YELLOW}OPTIMIZED"
		$CC ${FILES} -o ${BINARY_NAME} $COMPILE_OPT_FLAGS $LINK_OPT_FLAGS
fi
