// cola.h - Cola enlazada PROPIA de solicitudes de viaje (sin std::queue)
// FIFO: el primero que llega es el primero en atenderse.
// Mantenemos dos punteros: frente (por donde sale) y final (por donde entra).
#ifndef COLA_H
#define COLA_H
#include <string>

struct Solicitud {
    int idPasajero;
    std::string nombre;     // copia del nombre (así no depende de la lista)
    std::string origen;
    std::string destino;
};

struct NodoSolicitud {
    Solicitud dato;
    NodoSolicitud* sig;
};

class ColaSolicitudes {
private:
    NodoSolicitud* frente;
    NodoSolicitud* fin;
    int cantidad;
    ColaSolicitudes(const ColaSolicitudes&);
    ColaSolicitudes& operator=(const ColaSolicitudes&);
public:
    ColaSolicitudes();
    ~ColaSolicitudes();
    void vaciar();                          // elimina todas las solicitudes
    void encolar(const Solicitud& s);
    bool atender(Solicitud& salida);        // false si la cola está vacía (CP-03)
    bool verFrente(Solicitud& salida) const;
    void mostrar() const;
    bool vacia() const;
    int total() const;
};
#endif
