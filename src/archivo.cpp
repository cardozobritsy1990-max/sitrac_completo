// archivo.cpp - Persistencia resistente a archivo faltante o contenido corrupto
#include "archivo.h"
#include <fstream>
#include <cstdio>      // rename, remove
#include <stdexcept>
using namespace std;

bool guardarDatos(const string& ruta, const Grafo& g, const ListaPasajeros& l, string& error) {
    // Escribimos primero a un archivo temporal; solo si todo salio bien lo renombramos.
    // Asi un fallo a mitad de escritura no destruye el archivo anterior.
    string tmp = ruta + ".tmp";
    ofstream out(tmp.c_str());
    if (!out.is_open()) { error = "No se pudo crear el archivo '" + ruta + "'."; return false; }
    out << "SITRAC 1\n";
    out << "CIUDADES " << g.totalCiudades() << "\n";
    for (NodoCiudad* c = g.primera(); c != nullptr; c = c->sig) out << c->nombre << "\n";
    out << "CONEXIONES " << g.totalConexiones() << "\n";
    for (NodoCiudad* c = g.primera(); c != nullptr; c = c->sig)
        for (Arista* a = c->aristas; a != nullptr; a = a->sig)
            if (c->nombre < a->destino->nombre)            // cada conexion se escribe UNA vez
                out << c->nombre << "|" << a->destino->nombre << "|" << a->distancia << "\n";
    out << "PASAJEROS " << l.total() << "\n";
    for (const NodoPasajero* p = l.primero(); p != nullptr; p = p->sig)
        out << p->dato.id << "|" << p->dato.nombre << "|" << p->dato.origen << "|" << p->dato.destino << "\n";
    out << "FIN\n";
    out.close();
    if (out.fail()) { remove(tmp.c_str()); error = "Error al escribir en el disco."; return false; }
    if (rename(tmp.c_str(), ruta.c_str()) != 0) {          // en Windows rename falla si ya existe
        remove(ruta.c_str());
        if (rename(tmp.c_str(), ruta.c_str()) != 0) { error = "No se pudo reemplazar '" + ruta + "'."; return false; }
    }
    return true;
}

struct Lector { ifstream f; int n; Lector() : n(0) {} };   // n = numero de linea actual

static bool sigLinea(Lector& L, string& s) {
    if (!getline(L.f, s)) return false;
    L.n++;
    if (!s.empty() && s[s.size() - 1] == '\r') s.erase(s.size() - 1);   // archivos de Windows
    return true;
}
static bool fallo(Lector& L, string& error, const string& msg) {
    error = "Archivo corrupto (linea " + to_string(L.n) + "): " + msg;
    return false;
}
// Convierte a entero exigiendo que TODO el texto sea el numero.
static bool aEntero(const string& s, int& v) {
    try { size_t u = 0; int x = stoi(s, &u); if (u != s.size()) return false; v = x; return true; }
    catch (...) { return false; }
}
// Lee una linea "ETIQUETA n" y devuelve n (0..1000000).
static bool leerContador(Lector& L, const string& etiqueta, int& n, string& error) {
    string s;
    if (!sigLinea(L, s)) return fallo(L, error, "falta la seccion " + etiqueta + ".");
    string pref = etiqueta + " ";
    if (s.compare(0, pref.size(), pref) != 0) return fallo(L, error, "se esperaba '" + etiqueta + " n'.");
    if (!aEntero(s.substr(pref.size()), n) || n < 0 || n > 1000000) return fallo(L, error, "cantidad invalida en " + etiqueta + ".");
    return true;
}
// Divide una linea en EXACTAMENTE 'esperados' campos separados por '|'.
static bool partir(const string& linea, int esperados, string* campos) {
    int cuenta = 0; size_t ini = 0;
    for (;;) {
        size_t p = linea.find('|', ini);
        if (p == string::npos) {
            if (cuenta != esperados - 1) return false;     // faltaron campos
            campos[cuenta] = linea.substr(ini);
            return true;
        }
        if (cuenta >= esperados - 1) return false;         // sobraron campos
        campos[cuenta++] = linea.substr(ini, p - ini);
        ini = p + 1;
    }
}

// Lee TODO el archivo hacia las estructuras dadas, validando cada linea.
static bool leerEn(const string& ruta, Grafo& g, ListaPasajeros& l, ArbolABB& t, string& error) {
    Lector L;
    L.f.open(ruta.c_str());
    if (!L.f.is_open()) { error = "No se pudo abrir el archivo '" + ruta + "' (no existe o no se puede leer)."; return false; }
    string s;
    if (!sigLinea(L, s) || s != "SITRAC 1") return fallo(L, error, "encabezado invalido (se esperaba 'SITRAC 1').");

    int nc; if (!leerContador(L, "CIUDADES", nc, error)) return false;
    for (int i = 0; i < nc; i++) {
        if (!sigLinea(L, s)) return fallo(L, error, "faltan ciudades.");
        if (s.empty()) return fallo(L, error, "nombre de ciudad vacio.");
        if (!g.agregarCiudad(s)) return fallo(L, error, "ciudad duplicada '" + s + "'.");
    }
    int nk; if (!leerContador(L, "CONEXIONES", nk, error)) return false;
    for (int i = 0; i < nk; i++) {
        string c[3]; int d;
        if (!sigLinea(L, s)) return fallo(L, error, "faltan conexiones.");
        if (!partir(s, 3, c) || !aEntero(c[2], d)) return fallo(L, error, "formato de conexion invalido.");
        if (g.agregarConexion(c[0], c[1], d) != CON_OK)
            return fallo(L, error, "conexion invalida (ciudad inexistente, distancia <= 0, repetida o consigo misma).");
    }
    int np; if (!leerContador(L, "PASAJEROS", np, error)) return false;
    for (int i = 0; i < np; i++) {
        string c[4]; Pasajero p;
        if (!sigLinea(L, s)) return fallo(L, error, "faltan pasajeros.");
        if (!partir(s, 4, c) || !aEntero(c[0], p.id)) return fallo(L, error, "formato de pasajero invalido.");
        if (c[1].empty() || c[2].empty() || c[3].empty()) return fallo(L, error, "pasajero con campos vacios.");
        p.nombre = c[1]; p.origen = c[2]; p.destino = c[3];
        if (!l.insertar(p)) return fallo(L, error, "ID de pasajero duplicado.");
        t.insertar(p.id, p.nombre);
    }
    if (!sigLinea(L, s) || s != "FIN") return fallo(L, error, "falta la marca final 'FIN'.");
    while (sigLinea(L, s))
        if (!s.empty()) return fallo(L, error, "contenido extra despues de FIN.");
    return true;
}

// cargarDatos: DOS FASES. 1) validar cargando en estructuras temporales;
// 2) si todo fue valido, vaciar las reales y cargar de nuevo (ya sabemos que no fallara).
bool cargarDatos(const string& ruta, Grafo& g, ListaPasajeros& l, ArbolABB& t, string& error) {
    Grafo tg; ListaPasajeros tl; ArbolABB tt;
    if (!leerEn(ruta, tg, tl, tt, error)) return false;    // memoria real intacta
    g.vaciar(); l.vaciar(); t.vaciar();
    return leerEn(ruta, g, l, t, error);
}
