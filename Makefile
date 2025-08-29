source := src/*.cpp
application := TerminalSoftwareRasterizer

build:
	g++ -std=c++17 $(source) -o $(application)
