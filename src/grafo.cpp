// grafo con lista de adyacencia
#include "grafo.h"
#include <iostream>
#include <cctype>
using namespace std;

// Compara dos nombres sin distinguir mayúsculas ("Bogota" == "bogota").
static bool mismoNombre(const string& a, const string& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); i++)
        if (tolower((unsigned char)a[i]) != tolower((unsigned char)b[i])) return false;
    return true;
}

Grafo::Grafo() : cabeza(nullptr), nCiudades(0) {}

// Destructor: delega en vaciar().
Grafo::~Grafo() { vaciar(); }

// vaciar: libera las aristas de cada ciudad y luego la ciudad. Deja el grafo vacio.
void Grafo::vaciar() {
    while (cabeza != nullptr) {
        NodoCiudad* c = cabeza;
        cabeza = c->sig;
        while (c->aristas != nullptr) {
            Arista* a = c->aristas;
            c->aristas = a->sig;
            delete a;
        }
        delete c;
    }
    nCiudades = 0;
}

NodoCiudad* Grafo::primera() const { return cabeza; }

// anexarArista: agrega la arista AL FINAL de la lista del nodo 'desde'. Asi los vecinos
// se recorren en el orden en que se crearon las conexiones (resultados reproducibles).
void Grafo::anexarArista(NodoCiudad* desde, NodoCiudad* hacia, int distancia) {
    Arista* nueva = new Arista{hacia, distancia, nullptr};
    if (desde->aristas == nullptr) { desde->aristas = nueva; return; }
    Arista* a = desde->aristas;
    while (a->sig != nullptr) a = a->sig;
    a->sig = nueva;
}

NodoCiudad* Grafo::buscarCiudad(const string& nombre) const {
    for (NodoCiudad* c = cabeza; c != nullptr; c = c->sig)
        if (mismoNombre(c->nombre, nombre)) return c;
    return nullptr;
}

// agregarCiudad: si ya existe, NO crea ningún nodo (CP-01).
bool Grafo::agregarCiudad(const string& nombre) {
    if (buscarCiudad(nombre) != nullptr) return false;
    cabeza = new NodoCiudad{nombre, nullptr, cabeza, 0};   // inserción al inicio, O(1)
    nCiudades++;
    return true;
}

// quitarArista: borra la arista desde->hacia si existe (helper).
bool Grafo::quitarArista(NodoCiudad* desde, NodoCiudad* hacia) {
    Arista* previo = nullptr;
    for (Arista* a = desde->aristas; a != nullptr; previo = a, a = a->sig) {
        if (a->destino == hacia) {
            if (previo == nullptr) desde->aristas = a->sig;
            else previo->sig = a->sig;
            delete a;
            return true;
        }
    }
    return false;
}

// eliminarCiudad (CP-05): primero quita TODAS las aristas de otras ciudades que
// apuntan a ésta; así ningún puntero queda colgando. Luego la desenlaza y borra.
bool Grafo::eliminarCiudad(const string& nombre) {
    NodoCiudad* objetivo = buscarCiudad(nombre);
    if (objetivo == nullptr) return false;
    for (NodoCiudad* c = cabeza; c != nullptr; c = c->sig)
        if (c != objetivo) quitarArista(c, objetivo);   // limpia referencias entrantes
    while (objetivo->aristas != nullptr) {              // libera sus aristas salientes
        Arista* a = objetivo->aristas;
        objetivo->aristas = a->sig;
        delete a;
    }
    NodoCiudad* previo = nullptr;                       // desenlaza de la lista de ciudades
    for (NodoCiudad* c = cabeza; c != nullptr; previo = c, c = c->sig) {
        if (c == objetivo) {
            if (previo == nullptr) cabeza = c->sig;
            else previo->sig = c->sig;
            break;
        }
    }
    delete objetivo;
    nCiudades--;
    return true;
}

// agregarConexion: valida TODO antes de modificar el grafo.
ResultadoConexion Grafo::agregarConexion(const string& a, const string& b, int distancia) {
    NodoCiudad* ca = buscarCiudad(a);
    NodoCiudad* cb = buscarCiudad(b);
    if (ca == nullptr || cb == nullptr) return CON_CIUDAD_NO_EXISTE;
    if (ca == cb) return CON_MISMA_CIUDAD;
    if (distancia <= 0) return CON_DISTANCIA_INVALIDA;     // CP-04
    for (Arista* x = ca->aristas; x != nullptr; x = x->sig)
        if (x->destino == cb) return CON_YA_EXISTE;
    anexarArista(ca, cb, distancia);                       // A -> B
    anexarArista(cb, ca, distancia);                       // B -> A (no dirigido)
    return CON_OK;
}

bool Grafo::eliminarConexion(const string& a, const string& b) {
    NodoCiudad* ca = buscarCiudad(a);
    NodoCiudad* cb = buscarCiudad(b);
    if (ca == nullptr || cb == nullptr) return false;
    bool uno = quitarArista(ca, cb);
    bool dos = quitarArista(cb, ca);
    return uno && dos;                                     // false si la ruta no existía
}

bool Grafo::mostrarVecinos(const string& nombre) const {
    NodoCiudad* c = buscarCiudad(nombre);
    if (c == nullptr) return false;
    cout << "  Vecinos de " << c->nombre << ":\n";
    if (c->aristas == nullptr) cout << "    (sin conexiones)\n";
    for (Arista* a = c->aristas; a != nullptr; a = a->sig)
        cout << "    - " << a->destino->nombre << " (" << a->distancia << " km)\n";
    return true;
}

void Grafo::listarCiudades() const {
    if (cabeza == nullptr) { cout << "  (no hay ciudades)\n"; return; }
    for (NodoCiudad* c = cabeza; c != nullptr; c = c->sig) cout << "  - " << c->nombre << "\n";
}

int Grafo::totalCiudades() const { return nCiudades; }

// Cada conexión se guarda 2 veces, así que dividimos entre 2.
int Grafo::totalConexiones() const {
    int suma = 0;
    for (NodoCiudad* c = cabeza; c != nullptr; c = c->sig)
        for (Arista* a = c->aristas; a != nullptr; a = a->sig) suma++;
    return suma / 2;
}
