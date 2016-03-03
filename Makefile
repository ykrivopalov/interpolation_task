CXXFLAGS=--std=c++11 -DNDEBUG

interpolate: interpolation.cpp main.cpp
	g++ $(CXXFLAGS) interpolation.cpp main.cpp -o interpolate
