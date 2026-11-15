FLAGS = -c -std=c++11 -Wall -Werror


libsimpp.a:
	g++ $(FLAGS) src/*.cpp
	ar rcs libsimpp.a *.o
libsimpp_debug.a:
	g++ -g $(FLAGS) src/*.cpp
	ar rcs libsimpp.a *.o
