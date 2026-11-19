// pila.h - Pila enlazada PROPIA para el historial (LIFO, sin std::stack)
// El último evento registrado es el primero que se consulta o se retira.
#ifndef PILA_H
#define PILA_H
#include <string>

struct NodoHistorial {
    std::string evento;
    NodoHistorial* sig;     // apunta al evento anterior (más antiguo)
};

class PilaHistorial {
private:
    NodoHistorial* tope;
    int cantidad;
    PilaHistorial(const PilaHistorial&);
    PilaHistorial& operator=(const PilaHistorial&);
public:
    PilaHistorial();
    ~PilaHistorial();
    void apilar(const std::string& evento);
    bool desapilar(std::string& salida);   // false si está vacía
    bool verTope(std::string& salida) const;
    void mostrar() const;                  // del más reciente al más antiguo
    int total() const;
};
#endif
