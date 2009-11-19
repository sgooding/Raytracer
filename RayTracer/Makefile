CXX=g++
CXXFLAGS=-Weffc++ -ggdb
OPT = -O3 
LIBS=-lm -lANN
LIBDIR=-Lann/lib
CXXDIR=-Iann/include

raytrace: PhotonMapAnn.o PhotonMapHeinrik.o Vector.o Primitives.o RayTrace.o main.o 
	$(CXX) $(OPT) $^ -o $@ $(LIBDIR) $(LIBS) 

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CXXDIR) -c $< -o $@

clean: 
	rm *.o raytrace 

