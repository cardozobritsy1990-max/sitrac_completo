// rutas.cpp - BFS, DFS, componentes conexas y Dijkstra
// Truco de diseno: cada ciudad tiene un campo 'indice' (0..n-1) que asignamos al
// iniciar el algoritmo. Asi los arreglos auxiliares (visitado, distancia, ...) se
// consultan en O(1) y los recorridos cuestan O(V+E) de verdad.
#include "rutas.h"
#include <iostream>
using namespace std;

// Asigna indice 0..n-1 a cada ciudad y devuelve un arreglo con los punteros.
static NodoCiudad** numerarCiudades(const Grafo& g, int& n) {
    n = g.totalCiudades();
    NodoCiudad** v = new NodoCiudad*[n > 0 ? n : 1];
    int i = 0;
    for (NodoCiudad* c = g.primera(); c != nullptr; c = c->sig) { c->indice = i; v[i++] = c; }
    return v;
}

bool recorridoBFS(const Grafo& g, const string& origen) {
    NodoCiudad* o = g.buscarCiudad(origen);
    if (o == nullptr) return false;
    int n; NodoCiudad** v = numerarCiudades(g, n);
    bool* visitado = new bool[n];
    int* nivel = new int[n];
    int* cola = new int[n];                    // cola de indices; cada ciudad entra UNA vez
    for (int i = 0; i < n; i++) { visitado[i] = false; nivel[i] = 0; }
    int ini = 0, fin = 0;                      // 'ini' = frente, 'fin' = donde se encola
    visitado[o->indice] = true;                // se marca al ENCOLAR (evita repetidos)
    cola[fin++] = o->indice;
    cout << "  BFS desde " << o->nombre << ":\n";
    int orden = 0;
    while (ini < fin) {
        int u = cola[ini++];                   // atender el frente
        cout << "    " << ++orden << ". " << v[u]->nombre << " (nivel " << nivel[u] << ")\n";
        for (Arista* a = v[u]->aristas; a != nullptr; a = a->sig) {
            int w = a->destino->indice;
            if (!visitado[w]) {                // solo ciudades nuevas
                visitado[w] = true;
                nivel[w] = nivel[u] + 1;       // un nivel mas lejos que su "padre"
                cola[fin++] = w;
            }
        }
    }
    delete[] v; delete[] visitado; delete[] nivel; delete[] cola;
    return true;
}

// Recursion: la pila de llamadas hace de pila de DFS. 'contador' numera las visitas.
static void dfsRec(NodoCiudad* u, bool* visitado, int& contador) {
    visitado[u->indice] = true;
    cout << "    " << ++contador << ". " << u->nombre << "\n";
    for (Arista* a = u->aristas; a != nullptr; a = a->sig)
        if (!visitado[a->destino->indice]) dfsRec(a->destino, visitado, contador);
}

bool recorridoDFS(const Grafo& g, const string& origen) {
    NodoCiudad* o = g.buscarCiudad(origen);
    if (o == nullptr) return false;
    int n; NodoCiudad** v = numerarCiudades(g, n);
    bool* visitado = new bool[n];
    for (int i = 0; i < n; i++) visitado[i] = false;
    cout << "  DFS desde " << o->nombre << ":\n";
    int contador = 0;
    dfsRec(o, visitado, contador);
    delete[] v; delete[] visitado;
    return true;
}

void componentesConexas(const Grafo& g) {
    int n; NodoCiudad** v = numerarCiudades(g, n);
    if (g.totalCiudades() == 0) { cout << "  (no hay ciudades)\n"; delete[] v; return; }
    bool* visitado = new bool[n];
    for (int i = 0; i < n; i++) visitado[i] = false;
    int comp = 0;
    for (int i = 0; i < n; i++) {
        if (!visitado[i]) {                    // ciudad sin visitar = nueva componente
            cout << "  Componente " << ++comp << ":\n";
            int contador = 0;
            dfsRec(v[i], visitado, contador);  // visita todo lo alcanzable desde ella
        }
    }
    cout << "  Total de componentes: " << comp << "\n";
    delete[] v; delete[] visitado;
}

ResultadoRuta caminoMinimo(const Grafo& g, const string& origen, const string& destino) {
    NodoCiudad* o = g.buscarCiudad(origen);
    NodoCiudad* d = g.buscarCiudad(destino);
    if (o == nullptr || d == nullptr) return RUTA_CIUDAD_NO_EXISTE;
    int n; NodoCiudad** v = numerarCiudades(g, n);
    const long long INF = 1000000000000000LL;      // "infinito": aun no alcanzada
    long long* dist = new long long[n];            // mejor distancia conocida desde el origen
    int* previo = new int[n];                      // ciudad anterior en el mejor camino
    bool* definitivo = new bool[n];                // true cuando la distancia ya es la minima
    for (int i = 0; i < n; i++) { dist[i] = INF; previo[i] = -1; definitivo[i] = false; }
    dist[o->indice] = 0;
    for (int paso = 0; paso < n; paso++) {
        int u = -1;                                // elegir la ciudad NO definitiva mas cercana
        for (int i = 0; i < n; i++)
            if (!definitivo[i] && dist[i] < INF && (u == -1 || dist[i] < dist[u])) u = i;
        if (u == -1) break;                        // las restantes son inalcanzables
        definitivo[u] = true;
        for (Arista* a = v[u]->aristas; a != nullptr; a = a->sig) {   // relajacion
            int w = a->destino->indice;
            if (!definitivo[w] && dist[u] + a->distancia < dist[w]) {
                dist[w] = dist[u] + a->distancia;  // encontramos un camino mejor hacia w
                previo[w] = u;
            }
        }
    }
    ResultadoRuta res;
    if (dist[d->indice] == INF) {
        res = RUTA_SIN_CAMINO;
    } else {
        // Reconstruccion: seguimos 'previo' del destino hacia atras y luego invertimos.
        int largo = 0;
        for (int x = d->indice; x != -1; x = previo[x]) largo++;
        int* camino = new int[largo];
        int k = largo;
        for (int x = d->indice; x != -1; x = previo[x]) camino[--k] = x;   // llena de atras hacia adelante
        cout << "  Camino minimo " << o->nombre << " -> " << d->nombre << ":\n    ";
        for (int i = 0; i < largo; i++) cout << (i ? " -> " : "") << v[camino[i]]->nombre;
        cout << "\n  Distancia acumulada por ciudad:\n";
        for (int i = 0; i < largo; i++)
            cout << "    " << v[camino[i]]->nombre << ": " << dist[camino[i]] << " km\n";
        cout << "  Costo total: " << dist[d->indice] << " km\n";
        delete[] camino;
        res = RUTA_OK;
    }
    delete[] v; delete[] dist; delete[] previo; delete[] definitivo;
    return res;
}
