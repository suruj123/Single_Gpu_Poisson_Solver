#To compile and build the executable "main", execute "make" n the terminal if the name of
#the makefile is "Makefile", otherwise execute "make -f <filename>".

#Defining MACROS
SHELL   =/bin/bash
CC	= pgcc
#CC = gcc
#CFLAGS  = -fast
CFLAGS  = -fast -ta=tesla:cc60 -Minfo=accel
#CFLAGS  = -fast -ta=tesla:cc60, managed -Minfo=accel
#CFLAGS  = -acc -fast -ta=multicore -Minfo=accel
#CFLAGS = -w
LFLAG	= -lm 
OBJECTS = poission_solver_sor_gau_seidl.o
            
          
#Building target executable (main) form the latest object files
main	: $(OBJECTS) 
	$(CC) $(CFLAGS) -o main $(OBJECTS) $(LFLAG)

#Building object (.o) files from source (.c) files
.c.o:
	$(CC) $(CFLAGS) -c $*.c  $(LFLAG)


#Removing the executable (main) and the object files.
#Default value for the MACRO RM is "rm -f".
#To remove these files, we have to execute the command "make clean" in the terminal
#if the name of the makefile is "Makefile", else, if the filename is different from
#that of "Makefile", then execute "make clean -f <filename>".
 
clean :
	$(RM) main $(OBJECTS) 

