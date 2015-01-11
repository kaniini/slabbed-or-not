PROG = slabbed-or-not

${PROG}:
	cc -o ${PROG} ${PROG}.c

all: ${PROG}
default: all

clean:
	rm ${PROG}

