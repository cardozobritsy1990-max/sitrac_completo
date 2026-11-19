// arbol.h - Arbol Binario de Busqueda (ABB) PROPIO, indexado por ID de pasajero
// Regla ABB: todo ID del subarbol izquierdo es MENOR que el del nodo y todo ID
// del subarbol derecho es MAYOR. Por eso buscar/insertar/eliminar cuestan O(h),
// con h = altura (promedio O(log n); peor caso O(n) si se inserta en orden).
#ifndef ARBOL_H
#define ARBOL_H
#include <string>

struct NodoABB {
    int id;                 // clave de ordenamiento
    std::string nombre;     // dato asociado
    NodoABB* izq;           // subarbol de IDs menores
    NodoABB* der;           // subarbol de IDs mayores
};

class ArbolABB {
private:
    NodoABB* raiz;
    int cantidad;
    ArbolABB(const ArbolABB&);
    ArbolABB& operator=(const ArbolABB&);
public:
    ArbolABB();
    ~ArbolABB();
    bool insertar(int id, const std::string& nombre);   // false si el ID ya existe
    const NodoABB* buscar(int id) const;                // nullptr si no existe
    bool eliminar(int id);                              // false si no existe
    bool actualizarNombre(int id, const std::string& nombre);
    void inorden() const;       // izq, raiz, der  -> IDs en orden ascendente
    void preorden() const;      // raiz, izq, der
    void postorden() const;     // izq, der, raiz
    int altura() const;         // numero de niveles (arbol vacio = 0)
    int total() const;
    void vaciar();
};
#endif
