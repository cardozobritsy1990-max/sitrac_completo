// rutas.h - Algoritmos sobre el grafo: BFS, DFS, componentes y Dijkstra
#ifndef RUTAS_H
#define RUTAS_H
#include "grafo.h"
#include <string>

enum ResultadoRuta { RUTA_OK, RUTA_CIUDAD_NO_EXISTE, RUTA_SIN_CAMINO };

// BFS (anchura): visita por niveles usando una COLA. Complejidad O(V + E).
// Devuelve false si la ciudad de origen no existe.
bool recorridoBFS(const Grafo& g, const std::string& origen);

// DFS (profundidad): explora lo mas hondo posible (recursion = pila). O(V + E).
bool recorridoDFS(const Grafo& g, const std::string& origen);

// Componentes conexas: grupos de ciudades alcanzables entre si (DFS repetido). O(V + E).
void componentesConexas(const Grafo& g);

// Dijkstra: camino de menor distancia total (pesos no negativos). O(V^2) con arreglos.
ResultadoRuta caminoMinimo(const Grafo& g, const std::string& origen, const std::string& destino);
#endif
