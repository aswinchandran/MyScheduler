all: myScheduler

myScheduler: main.o libmyScheduler.a #libmylib.a is the dependency for the executable
        gcc -lm -o myScheduler main.o -L. -libmyScheduler

main.o: main.c
        gcc -O -c main.c sch_os.h

sch_os.o: sch_os.c sch_os.h
        gcc -O -c sch_os.c

scheduler.o: scheduler.c sch_os.h
        gcc -O -c scheduler.c

libmyScheduler.a: sch_os.o scheduler.o #let's link library files into a static library
        ar rcs libmyScheduler.a sch_os.o scheduler.o

libs: libmyScheduler.a

clean:
        rm -f myScheduler *.o *.a *.gch #This way is cleaner than your clean