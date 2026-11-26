FLAGS = -c -std=c++14 -Wall -Werror
VERSION = 0.0.1-testing
ifeq ($(OS),Windows_NT)
	DETECTED_OS := Windows
else
	DETECTED_OS := $(shell uname -s)
endif



libsimpp.a:
	g++ $(FLAGS) src/*.cpp
	ar rcs libsimpp.a *.o
libsimpp_debug.a:
	g++ -g $(FLAGS) src/*.cpp
	ar rcs libsimpp.a *.o

linux_build: libsimpp.a
	mkdir ./build/temp
	mkdir ./build/temp/libsimpp_$(VERSION)_linux
	mv libsimpp.a ./build/temp/libsimpp_$(VERSION)_linux/
	cp ./src/*.hpp ./build/temp/libsimpp_$(VERSION)_linux/
	tar -cJvf ./build/libsimpp_$(VERSION)_linux.tar.xz -C ./build/temp/ .
	rm -rf ./build/temp
	rm *.o
macOS_build: libsimpp.a
ifeq ($(DETECTED_OS),Darwin)
	mkdir ./build/temp
	mkdir ./build/temp/libsimpp_$(VERSION)_macOS
	mv libsimpp.a ./build/temp/libsimpp_$(VERSION)_macOS/
	cp ./src/*.hpp ./build/temp/libsimpp_$(VERSION)_macOS/
	tar -cJvf ./build/libsimpp_$(VERSION)_macOS.tar.xz -C ./build/temp/ .
	rm -rf ./build/temp
	rm *.o
else
	@echo "Unimplemented"
endif
