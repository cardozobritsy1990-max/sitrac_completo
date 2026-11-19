#ifndef ARCHIVO_H
#define ARCHIVO_H
#include <string>
#include "grafo.h"
#include "pasajeros.h"
#include "arbol.h"

// Guarda ciudades, conexiones y pasajeros. false + 'error' si no se pudo escribir.
bool guardarDatos(const std::string& ruta, const Grafo& g, const ListaPasajeros& l, std::string& error);

// Carga reemplazando el contenido actual. SOLO modifica la memoria si el archivo
// completo es valido; si falta o esta corrupto devuelve false y no cambia nada.
bool cargarDatos(const std::string& ruta, Grafo& g, ListaPasajeros& l, ArbolABB& t, std::string& error);
#endif
