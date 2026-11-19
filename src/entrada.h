#ifndef ENTRADA_H
#define ENTRADA_H

#include <string>

// Pide un entero. Repite la pregunta hasta que el usuario escriba un número
// válido. Devuelve true si obtuvo el número; devuelve false SOLO si la entrada
// se cerró (fin de archivo, Ctrl+D), para que quien llame pueda salir limpio.
bool leerEntero(const std::string& mensaje, int& valor);

// Igual que leerEntero, pero además exige que el número esté en [min, max].
bool leerEnteroEnRango(const std::string& mensaje, int min, int max, int& valor);

// Pide un texto no vacío (se recortan espacios al inicio y al final).
// Devuelve false solo si la entrada se cerró.
bool leerTexto(const std::string& mensaje, std::string& texto);

#endif
