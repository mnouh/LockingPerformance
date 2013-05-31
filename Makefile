GXX =  gcc -g -Wall #-ansi -pedantic

all: parta  partb_busywait partb_sched_yield partb_sem partc

parta.o: parta.c
	$(GXX) -c parta.c -o parta.o

parta: parta.o
	$(GXX) parta.o -lrt -o parta

partb_busywait.o: partb_busywait.c
	$(GXX) -c partb_busywait.c -o partb_busywait.o

partb_busywait: partb_busywait.o
	$(GXX) partb_busywait.o -lrt -o partb_busywait

partb_sched_yield: partb_sched_yield.o
	$(GXX) partb_sched_yield.o -lrt -o partb_sched_yield

partb_sched_yield.o: partb_sched_yield.c
	$(GXX) -c partb_sched_yield.c -o  partb_sched_yield.o

partb_sem: partb_sem.o
	$(GXX) partb_sem.o -lrt -o partb_sem

partb_sem.o: partb_sem.c
	$(GXX) -c partb_sem.c -o partb_sem.o

partc: partc.o
	$(GXX) partc.o -lrt -o partc

part_c.o: partc.c
	$(GXX) -c partc.c -o partc.o


clean:
	rm -r *.o parta partb_sem partb_busywait partb_sched_yield partc
