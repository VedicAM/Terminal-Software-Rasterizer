source := src/*.cpp
application := main

build:
	g++ -std=c++17 $(source) -o $(application)