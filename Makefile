CC = gcc
CFLAGS = -Wall -g -pthread

PROGRAMS = modus_sem \
modus_sem_enxuto \
modus_barr \
modus_all

all: $(PROGRAMS)

clean:
	rm -f *.o *~ $(PROGRAMS)
