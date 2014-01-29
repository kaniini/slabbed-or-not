PROG = slabbed-or-not

${PROG}:
	gcc -o ${PROG} ${PROG}.c

all: ${PROG}
default: all

clean:
	rm ${PROG}

