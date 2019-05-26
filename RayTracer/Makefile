CXX=g++
CXXFLAGS=-Weffc++ -ggdb
OPT = -O3 
LIBS=-lm -lANN
LIBDIR=-Lthird/ann/lib
CXXDIR=-Ithird/ann/include

raytrace: PhotonMapAnn.o Vector.o Primitives.o RayTrace.o main.o 
	$(CXX) $(OPT) $^ -o $@ $(LIBDIR) $(LIBS) 

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CXXDIR) -c $< -o $@

clean: 
	rm *.o raytrace 

