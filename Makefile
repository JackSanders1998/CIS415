CC = gcc
CFLAGS = -g -Wall

a.out: part1.out part2.out part3.out part4.out cpubound iobound

part1.out: part1.c
	$(CC) $(CFLAGS) part1.c -o part1.out

part2.out: part2.c
	$(CC) $(CFLAGS) part2.c -o part2.out

part3.out: part3.c
	$(CC) $(CFLAGS) part1.c -o part1.out

part4.out: part4.c
	$(CC) $(CFLAGS) part4.c -o part4.out

cpubound: cpubound.c
	$(CC) $(CFLAGS) cpubound.c -o cpubound

iobound: iobound.c
	$(CC) $(CFLAGS) iobound.c -o iobound

clean:
	$(RM) cpubound iobound *.out *~
