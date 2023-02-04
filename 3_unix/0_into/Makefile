CXX=g++
CXXFLAGS=-std=c++17
LDLIBS=-lpthread
DEPS = 
OBJ =

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LDLIBS)

all : helloworld standarts

standarts: standarts.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

helloworld: helloworld.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

