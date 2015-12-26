LDFLAGS = -lboost_program_options -lboost_system -lboost_filesystem -lprotobuf
CXXFLAGS = -std=c++11

all: tracer

tracer: tracer.o Trace.pb.o
	$(CXX) -o tracer tracer.o Trace.pb.o $(LDFLAGS)

Trace.pb.o: Trace.pb.h
	$(CXX) -c -o Trace.pb.o Trace.pb.cc

Trace.pb.h: ../schema/Trace.proto
	protoc -I../schema --cpp_out=. ../schema/Trace.proto

tracer.o: tracer.cpp Trace.pb.h
	$(CXX) $(CXXFLAGS) -o tracer.o -c tracer.cpp 

clean:
	rm -f tracer tracer.o Trace.pb.o Trace.pb.h
