all: tracer

tracer: tracer.o 
	$(CXX) -o tracer tracer.o

tracer.o: tracer.cpp
	$(CXX) -o tracer.o -c tracer.cpp 

clean:
	rm -f tracer tracer.o
