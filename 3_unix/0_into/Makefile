CXX=g++
CXXFLAGS=-std=c++17
LDLIBS=-lpthread
DEPS = 
OBJ =

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(LDLIBS)

all : helloworld standarts fileio fildir standartio sysinfo proccenv procctrl

standarts: standarts.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

helloworld: helloworld.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

fileio: fileio.o 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

fildir: fildir.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

standartio: standartio.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

sysinfo: sysinfo.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)

proccenv: proccenv.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)
	
procctrl: procctrl.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDLIBS)
