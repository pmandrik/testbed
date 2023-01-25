CXX=g++
CXXFLAGS=-std=c++11
LDLIBS=-lpthread
DEPS = 
OBJ = helloworld.o 

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LDLIBS)

helloworld: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)
  
