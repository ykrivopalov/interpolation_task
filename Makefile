CXXFLAGS=--std=c++11

interpolate: interpolation.cpp main.cpp
	g++ $(CXXFLAGS) interpolation.cpp main.cpp -o interpolate
