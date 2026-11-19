#include "arbol.h"
#include <iostream>
using namespace std;

static NodoABB* insertarRec(NodoABB* nodo, int id, const string& nombre, bool& insertado) {
    if (nodo == nullptr) {                       // hueco encontrado: crear nodo hoja
        insertado = true;
        return new NodoABB{id, nombre, nullptr, nullptr};
    }
    if (id < nodo->id)      nodo->izq = insertarRec(nodo->izq, id, nombre, insertado);
    else if (id > nodo->id) nodo->der = insertarRec(nodo->der, id, nombre, insertado);
    else                    insertado = false;   // ID repetido: no se inserta
    return nodo;
}

static NodoABB* eliminarRec(NodoABB* nodo, int id, bool& eliminado) {
    if (nodo == nullptr) return nullptr;         // no existe
    if (id < nodo->id) { nodo->izq = eliminarRec(nodo->izq, id, eliminado); return nodo; }
    if (id > nodo->id) { nodo->der = eliminarRec(nodo->der, id, eliminado); return nodo; }
    eliminado = true;                            // lo encontramos
    if (nodo->izq == nullptr) {                  // caso hoja o solo hijo derecho
        NodoABB* hijo = nodo->der;
        delete nodo;
        return hijo;                             // el hijo "sube" a ocupar su lugar
    }
    if (nodo->der == nullptr) {                  // solo hijo izquierdo
        NodoABB* hijo = nodo->izq;
        delete nodo;
        return hijo;
    }
    // Dos hijos: el sucesor es el menor del subarbol derecho (el mas a la izquierda).
    NodoABB* suc = nodo->der;
    while (suc->izq != nullptr) suc = suc->izq;
    nodo->id = suc->id;                          // copiamos los datos del sucesor aqui
    nodo->nombre = suc->nombre;
    bool aux = false;
    nodo->der = eliminarRec(nodo->der, suc->id, aux);   // y eliminamos el sucesor original
    return nodo;
}

static int alturaRec(const NodoABB* n) {
    if (n == nullptr) return 0;
    int hi = alturaRec(n->izq), hd = alturaRec(n->der);
    return 1 + (hi > hd ? hi : hd);
}

// Recorridos: imprimen los IDs separados por coma. 'primero' controla la coma inicial.
static void inordenRec(const NodoABB* n, bool& primero) {
    if (!n) return;
    inordenRec(n->izq, primero);
    cout << (primero ? "" : ", ") << n->id; primero = false;
    inordenRec(n->der, primero);
}
static void preordenRec(const NodoABB* n, bool& primero) {
    if (!n) return;
    cout << (primero ? "" : ", ") << n->id; primero = false;
    preordenRec(n->izq, primero);
    preordenRec(n->der, primero);
}
static void postordenRec(const NodoABB* n, bool& primero) {
    if (!n) return;
    postordenRec(n->izq, primero);
    postordenRec(n->der, primero);
    cout << (primero ? "" : ", ") << n->id; primero = false;
}

static void liberarRec(NodoABB* n) {            // postorden: hijos primero, luego el nodo
    if (!n) return;
    liberarRec(n->izq);
    liberarRec(n->der);
    delete n;
}

ArbolABB::ArbolABB() : raiz(nullptr), cantidad(0) {}
ArbolABB::~ArbolABB() { vaciar(); }
void ArbolABB::vaciar() { liberarRec(raiz); raiz = nullptr; cantidad = 0; }

bool ArbolABB::insertar(int id, const string& nombre) {
    bool ok = false;
    raiz = insertarRec(raiz, id, nombre, ok);
    if (ok) cantidad++;
    return ok;
}

// buscar: version iterativa, O(h).
const NodoABB* ArbolABB::buscar(int id) const {
    const NodoABB* act = raiz;
    while (act != nullptr) {
        if (id == act->id) return act;
        act = (id < act->id) ? act->izq : act->der;
    }
    return nullptr;
}

bool ArbolABB::eliminar(int id) {
    bool ok = false;
    raiz = eliminarRec(raiz, id, ok);
    if (ok) cantidad--;
    return ok;
}

bool ArbolABB::actualizarNombre(int id, const string& nombre) {
    NodoABB* act = raiz;
    while (act != nullptr) {
        if (id == act->id) { act->nombre = nombre; return true; }
        act = (id < act->id) ? act->izq : act->der;
    }
    return false;
}

void ArbolABB::inorden() const {
    cout << "  Inorden: ";
    if (!raiz) { cout << "(arbol vacio)\n"; return; }
    bool p = true; inordenRec(raiz, p); cout << "\n";
}
void ArbolABB::preorden() const {
    cout << "  Preorden: ";
    if (!raiz) { cout << "(arbol vacio)\n"; return; }
    bool p = true; preordenRec(raiz, p); cout << "\n";
}
void ArbolABB::postorden() const {
    cout << "  Postorden: ";
    if (!raiz) { cout << "(arbol vacio)\n"; return; }
    bool p = true; postordenRec(raiz, p); cout << "\n";
}
int ArbolABB::altura() const { return alturaRec(raiz); }
int ArbolABB::total() const { return cantidad; }
