# Compilar: make   |   Ejecutar: ./sitrac   |   Pruebas: make test
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
SRC = $(wildcard src/*.cpp)
sitrac: $(SRC) $(wildcard src/*.h)
	$(CXX) $(CXXFLAGS) -o sitrac $(SRC)
test: sitrac
	bash pruebas.sh
