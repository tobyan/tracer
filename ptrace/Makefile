LDFLAGS = -lboost_program_options -lboost_system -lboost_filesystem
CXXFLAGS = -std=c++11

all: tracer

tracer: tracer.o 
	$(CXX) -o tracer tracer.o $(LDFLAGS)

tracer.o: tracer.cpp
	$(CXX) $(CXXFLAGS) -o tracer.o -c tracer.cpp 

clean:
	rm -f tracer tracer.o
