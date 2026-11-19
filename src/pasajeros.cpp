// pasajeros.cpp - Implementación de la lista enlazada de pasajeros
#include "pasajeros.h"
#include <iostream>
using namespace std;

ListaPasajeros::ListaPasajeros() : cabeza(nullptr), cantidad(0) {}

// Destructor: delega en vaciar().
ListaPasajeros::~ListaPasajeros() { vaciar(); }

// vaciar: recorre la lista y borra cada nodo para no dejar fugas de memoria.
void ListaPasajeros::vaciar() {
    NodoPasajero* act = cabeza;
    while (act != nullptr) {
        NodoPasajero* siguiente = act->sig;   // guardamos el siguiente ANTES de borrar
        delete act;
        act = siguiente;
    }
    cabeza = nullptr;
    cantidad = 0;
}

const NodoPasajero* ListaPasajeros::primero() const { return cabeza; }

// insertar: añade al final. Primero verifica que el ID no esté repetido.
bool ListaPasajeros::insertar(const Pasajero& p) {
    if (buscar(p.id) != nullptr) return false;     // ID duplicado -> rechazar
    NodoPasajero* nuevo = new NodoPasajero{p, nullptr};
    if (cabeza == nullptr) {                       // lista vacía: es el primero
        cabeza = nuevo;
    } else {
        NodoPasajero* act = cabeza;
        while (act->sig != nullptr) act = act->sig; // vamos hasta el último
        act->sig = nuevo;                           // lo enlazamos al final
    }
    cantidad++;
    return true;
}

// buscar: recorrido lineal O(n). Devuelve puntero al dato o nullptr.
const Pasajero* ListaPasajeros::buscar(int id) const {
    for (NodoPasajero* act = cabeza; act != nullptr; act = act->sig)
        if (act->dato.id == id) return &act->dato;
    return nullptr;
}

// actualizar: modifica los datos (el ID no cambia).
bool ListaPasajeros::actualizar(int id, const string& nombre,
                                const string& origen, const string& destino) {
    for (NodoPasajero* act = cabeza; act != nullptr; act = act->sig) {
        if (act->dato.id == id) {
            act->dato.nombre = nombre;
            act->dato.origen = origen;
            act->dato.destino = destino;
            return true;
        }
    }
    return false;                                  // no existía
}

// eliminar: reconecta los enlaces ANTES de borrar el nodo, para no romper la lista.
bool ListaPasajeros::eliminar(int id) {
    NodoPasajero* previo = nullptr;
    NodoPasajero* act = cabeza;
    while (act != nullptr && act->dato.id != id) { // buscamos el nodo y su anterior
        previo = act;
        act = act->sig;
    }
    if (act == nullptr) return false;              // CP-02: inexistente, no hace nada
    if (previo == nullptr) cabeza = act->sig;      // era el primero
    else previo->sig = act->sig;                   // saltamos el nodo eliminado
    delete act;
    cantidad--;
    return true;
}

void ListaPasajeros::listar() const {
    if (cabeza == nullptr) { cout << "  (no hay pasajeros)\n"; return; }
    for (NodoPasajero* act = cabeza; act != nullptr; act = act->sig)
        cout << "  ID " << act->dato.id << " | " << act->dato.nombre
             << " | " << act->dato.origen << " -> " << act->dato.destino << "\n";
}

int ListaPasajeros::total() const { return cantidad; }
