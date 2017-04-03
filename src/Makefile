FLAGS = -Wall -ansi -g
OBJS = simulator.o system.o processor.o

cpusim : $(OBJS)
	g++ $(FLAGS) $(OBJS) -o cpusim

simulator.o : simulator.cpp
	g++ $(FLAGS) -c simulator.cpp

system.o : system.cpp
	g++ $(FLAGS) -c system.cpp

processor.o : processor.cpp
	g++ $(FLAGS) -c processor.cpp

clean :
	rm -f simulator
	rm -rf *.o
	rm -f *~ *.h.gch *#
