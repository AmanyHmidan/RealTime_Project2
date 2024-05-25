CC = gcc
LIBSPARENT = -lGL -lGLU -lglut 
LIBSLAST = -lglut -lGLU -lGL
LIBSPLAYER = -lm

all: main cargo_plane collectingTeam splittingTeam distributingTeam family distributingWorker collectingWorker splittingWorker occupationForces Drawer

main: main.o shared_mem_utils.o
	$(CC) -o main main.o shared_mem_utils.o $(LIBSPARENT)
	
Drawer: Drawer.c
	$(CC)  -o Drawer Drawer.c $(LIBSLAST)

cargo_plane: cargo_plane.o shared_mem_utils.o
	$(CC) -o cargo_plane cargo_plane.o shared_mem_utils.o $(LIBSPLAYER)
	
collectingTeam: collectingTeam.o shared_mem_utils.o
	$(CC) -o collectingTeam collectingTeam.o shared_mem_utils.o $(LIBSPLAYER)
	
splittingTeam: splittingTeam.o shared_mem_utils.o
	$(CC) -o splittingTeam splittingTeam.o shared_mem_utils.o $(LIBSPLAYER)
	
distributingTeam: distributingTeam.o shared_mem_utils.o
	$(CC) -o distributingTeam distributingTeam.o shared_mem_utils.o $(LIBSPLAYER)
	
family: family.o shared_mem_utils.o
	$(CC) -o family family.o shared_mem_utils.o $(LIBSPLAYER)
	
occupationForces: occupationForces.o  
	$(CC) -o occupationForces occupationForces.o $(LIBSPLAYER)	
		
main.o: main.c
	$(CC) -c -o main.o main.c

cargo_plane.o: cargo_plane.c
	$(CC) -c -o cargo_plane.o cargo_plane.c
	
collectingTeam.o: collectingTeam.c
	$(CC) -c -o collectingTeam.o collectingTeam.c
	
splittingTeam.o: splittingTeam.c
	$(CC) -c -o splittingTeam.o splittingTeam.c
	
distributingTeam.o: distributingTeam.c
	$(CC) -c -o distributingTeam.o distributingTeam.c	
        
shared_mem_utils.o: shared_mem_utils.c
	$(CC) -c -o shared_mem_utils.o shared_mem_utils.c
	
family.o: family.c
	$(CC) -c -o family.o family.c	

distributingWorker.o: distributingWorker.c
	$(CC) -c -o distributingWorker.o distributingWorker.c		

collectingWorker.o: collectingWorker.c
	$(CC) -c -o collectingWorker.o collectingWorker.c

splittingWorker.o: splittingWorker.c
	$(CC) -c -o splittingWorker.o splittingWorker.c
	
occupationForces.o: occupationForces.c
	$(CC) -c -o occupationForces.o occupationForces.c
clean:
	rm -f main cargo_plane collectingTeam splittingTeam distributingTeam family distributingWorker collectingWorker splittingWorker occupationForces Drawer *.o

