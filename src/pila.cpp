// pila.cpp - Implementación de la pila enlazada
#include "pila.h"
#include <iostream>
using namespace std;

PilaHistorial::PilaHistorial() : tope(nullptr), cantidad(0) {}

PilaHistorial::~PilaHistorial() {
    string basura;
    while (desapilar(basura)) {}
}

// apilar: el nuevo nodo pasa a ser el tope y apunta al tope anterior.
void PilaHistorial::apilar(const string& evento) {
    tope = new NodoHistorial{evento, tope};
    cantidad++;
}

bool PilaHistorial::desapilar(string& salida) {
    if (tope == nullptr) return false;     // pila vacía: no hay nada que sacar
    NodoHistorial* viejo = tope;
    salida = viejo->evento;
    tope = tope->sig;
    delete viejo;
    cantidad--;
    return true;
}

bool PilaHistorial::verTope(string& salida) const {
    if (tope == nullptr) return false;
    salida = tope->evento;
    return true;
}

void PilaHistorial::mostrar() const {
    if (tope == nullptr) { cout << "  (historial vacio)\n"; return; }
    for (NodoHistorial* a = tope; a != nullptr; a = a->sig)
        cout << "  - " << a->evento << "\n";
}

int PilaHistorial::total() const { return cantidad; }
