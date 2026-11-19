// pasajeros.h - Lista enlazada PROPIA de pasajeros (sin std::list)
// Cada pasajero es un nodo; los nodos se enlazan con punteros "sig".
#ifndef PASAJEROS_H
#define PASAJEROS_H
#include <string>

// Datos de un pasajero.
struct Pasajero {
    int id;                 // identificador único
    std::string nombre;
    std::string origen;     // ciudad de origen
    std::string destino;    // ciudad de destino
};

// Nodo de la lista: guarda un pasajero y apunta al siguiente nodo.
struct NodoPasajero {
    Pasajero dato;
    NodoPasajero* sig;
};

class ListaPasajeros {
private:
    NodoPasajero* cabeza;   // primer nodo (nullptr si la lista está vacía)
    int cantidad;           // cuántos pasajeros hay
    // Evitamos copias accidentales (copiar punteros crudos duplicaría nodos)
    ListaPasajeros(const ListaPasajeros&);
    ListaPasajeros& operator=(const ListaPasajeros&);
public:
    ListaPasajeros();
    ~ListaPasajeros();                         // libera TODOS los nodos
    void vaciar();                             // deja la lista vacia
    const NodoPasajero* primero() const;       // para recorrer la lista desde fuera
    bool insertar(const Pasajero& p);          // false si el ID ya existe
    const Pasajero* buscar(int id) const;      // nullptr si no existe
    bool actualizar(int id, const std::string& nombre,
                    const std::string& origen, const std::string& destino);
    bool eliminar(int id);                     // false si no existe
    void listar() const;                       // imprime todos
    int total() const;
};
#endif
