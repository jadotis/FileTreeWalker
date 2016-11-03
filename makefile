all:
	make clean
	make directoryLib
	make main
	
	
directoryLib:
	gcc -ggdb -c -o  Directories.o Directories.c
	ar rcs Directories.a Directories.o 	

main: 
	gcc -ggdb finds.c Directories.a -o finds
	gcc -ggdb -o finds.o finds.c Directories.a

clean:
	rm finds || true
	rm Directories.a || true
	rm Directories.o || true
	
