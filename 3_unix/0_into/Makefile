CXX=g++
CXXFLAGS=-std=c++17
LDLIBS=-lpthread
DEPS = 
OBJ =

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LDLIBS)

all : helloworld

helloworld: helloworld.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

