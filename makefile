all: test6

test6: test6.o graph.o
	g++ -o test6 test6.o graph.o

test6.o: test6.cpp
	g++ -c test6.cpp

graph.o: graph.cpp
	g++ -c graph.cpp

clean:
	rm -f test6 *.o

run: test6
	./test6