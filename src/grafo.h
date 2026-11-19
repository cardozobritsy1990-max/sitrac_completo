// grafo.h - Grafo NO dirigido con LISTA DE ADYACENCIA propia
// Estructura: lista enlazada de ciudades; cada ciudad tiene su propia lista
// enlazada de aristas (conexiones) con la distancia. Una conexión A-B se
// guarda dos veces: A->B y B->A.
#ifndef GRAFO_H
#define GRAFO_H
#include <string>

struct NodoCiudad;   // declaración adelantada (Arista apunta a una ciudad)

struct Arista {
    NodoCiudad* destino;   // ciudad a la que llega la conexión
    int distancia;         // km (siempre > 0)
    Arista* sig;
};

struct NodoCiudad {
    std::string nombre;
    Arista* aristas;       // lista de conexiones salientes
    NodoCiudad* sig;       // siguiente ciudad en el grafo
    int indice;            // posicion temporal (0..n-1) que usan BFS/DFS/Dijkstra
};

// Resultado posible al crear una conexión (para mostrar el mensaje correcto).
enum ResultadoConexion {
    CON_OK, CON_CIUDAD_NO_EXISTE, CON_DISTANCIA_INVALIDA,
    CON_MISMA_CIUDAD, CON_YA_EXISTE
};

class Grafo {
private:
    NodoCiudad* cabeza;
    int nCiudades;
    Grafo(const Grafo&);
    Grafo& operator=(const Grafo&);
    bool quitarArista(NodoCiudad* desde, NodoCiudad* hacia);   // helper privado
    void anexarArista(NodoCiudad* desde, NodoCiudad* hacia, int distancia); // agrega AL FINAL
public:
    Grafo();
    ~Grafo();
    bool agregarCiudad(const std::string& nombre);   // false si ya existe (CP-01)
    bool eliminarCiudad(const std::string& nombre);  // borra también sus conexiones (CP-05)
    NodoCiudad* buscarCiudad(const std::string& nombre) const;
    ResultadoConexion agregarConexion(const std::string& a, const std::string& b, int distancia);
    bool eliminarConexion(const std::string& a, const std::string& b);
    bool mostrarVecinos(const std::string& nombre) const;
    void listarCiudades() const;
    NodoCiudad* primera() const;    // primera ciudad (para recorrer el grafo desde fuera)
    void vaciar();                  // libera todo el grafo
    int totalCiudades() const;
    int totalConexiones() const;
};
#endif
