CXX=g++
CXXFLAGS=-std=c++17
LDLIBS=-lpthread
DEPS = 
OBJ =

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LDLIBS)

all : helloworld race sync

helloworld: helloworld.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

race: race.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)
  
sync: sync.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)


