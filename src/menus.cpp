// menus.cpp - Menus de SITRAC conectados a las estructuras REALES
// Cada submenu es una funcion aparte; main() no tiene logica.
// Convencion: cada submenu devuelve false SOLO si la entrada se cerro (Ctrl+D),
// para que el programa pueda terminar limpio sin quedar en un ciclo infinito.
#include "menus.h"
#include "entrada.h"
#include "grafo.h"
#include "pasajeros.h"
#include "cola.h"
#include "pila.h"
#include "arbol.h"
#include "rutas.h"
#include "archivo.h"
#include <iostream>
using namespace std;

enum OpcionPrincipal {
    OP_SALIR = 0, OP_CIUDADES = 1, OP_CONEXIONES = 2, OP_PASAJEROS = 3, OP_COLA = 4,
    OP_HISTORIAL = 5, OP_ARBOL = 6, OP_RUTAS = 7, OP_REPORTES = 8, OP_ARCHIVOS = 9
};

// Agrupa todas las estructuras del sistema; se pasa por referencia a los submenus.
struct Sistema {
    Grafo grafo;                 // ciudades y conexiones
    ListaPasajeros pasajeros;    // lista enlazada de pasajeros
    ColaSolicitudes cola;        // solicitudes de viaje (FIFO)
    PilaHistorial historial;     // eventos (LIFO)
    ArbolABB arbol;              // indice de pasajeros por ID
};

static bool menuCiudades(Sistema& s) {
    while (true) {
        cout << "\n--- Gestion de ciudades ---\n"
                "  1. Registrar ciudad\n  2. Eliminar ciudad\n  3. Listar ciudades\n  0. Volver\n";
        int op; if (!leerEnteroEnRango("Opcion: ", 0, 3, op)) return false;
        if (op == 0) return true;
        if (op == 3) { s.grafo.listarCiudades(); continue; }
        string nombre;
        if (!leerTexto("Nombre de la ciudad: ", nombre)) return false;
        if (op == 1) {
            if (s.grafo.agregarCiudad(nombre)) {
                cout << "  [OK] Ciudad registrada.\n";
                s.historial.apilar("Ciudad registrada: " + nombre);
            } else cout << "  [!] La ciudad '" << nombre << "' ya existe. No se creo nada.\n";   // CP-01
        } else {
            if (s.grafo.eliminarCiudad(nombre)) {
                cout << "  [OK] Ciudad y sus conexiones eliminadas.\n";                          // CP-05
                s.historial.apilar("Ciudad eliminada: " + nombre);
            } else cout << "  [!] La ciudad no existe.\n";
        }
    }
}

static bool menuConexiones(Sistema& s) {
    while (true) {
        cout << "\n--- Gestion de conexiones ---\n"
                "  1. Crear conexion\n  2. Eliminar conexion\n  3. Consultar vecinos\n  0. Volver\n";
        int op; if (!leerEnteroEnRango("Opcion: ", 0, 3, op)) return false;
        if (op == 0) return true;
        string a;
        if (!leerTexto(op == 3 ? "Ciudad: " : "Ciudad A: ", a)) return false;
        if (op == 3) {
            if (!s.grafo.mostrarVecinos(a)) cout << "  [!] La ciudad no existe.\n";
            continue;
        }
        string b;
        if (!leerTexto("Ciudad B: ", b)) return false;
        if (op == 1) {
            int d; if (!leerEntero("Distancia (km): ", d)) return false;
            switch (s.grafo.agregarConexion(a, b, d)) {
                case CON_OK:
                    cout << "  [OK] Conexion creada.\n";
                    s.historial.apilar("Conexion creada: " + a + " - " + b);
                    break;
                case CON_CIUDAD_NO_EXISTE:    cout << "  [!] Una de las ciudades no existe.\n"; break;
                case CON_DISTANCIA_INVALIDA:  cout << "  [!] La distancia debe ser mayor que 0.\n"; break;   // CP-04
                case CON_MISMA_CIUDAD:        cout << "  [!] Una ciudad no se conecta consigo misma.\n"; break;
                case CON_YA_EXISTE:           cout << "  [!] Esa conexion ya existe.\n"; break;
            }
        } else {
            if (s.grafo.eliminarConexion(a, b)) {
                cout << "  [OK] Conexion eliminada.\n";
                s.historial.apilar("Conexion eliminada: " + a + " - " + b);
            } else cout << "  [!] Esa ruta no existe.\n";
        }
    }
}

// La lista es la fuente de verdad; el ABB es un indice por ID que se mantiene
// sincronizado en registrar / actualizar / eliminar.
static bool menuPasajeros(Sistema& s) {
    while (true) {
        cout << "\n--- Gestion de pasajeros ---\n"
                "  1. Registrar\n  2. Buscar\n  3. Actualizar\n  4. Eliminar\n  5. Listar\n  0. Volver\n";
        int op; if (!leerEnteroEnRango("Opcion: ", 0, 5, op)) return false;
        if (op == 0) return true;
        if (op == 5) { s.pasajeros.listar(); continue; }
        int id; if (!leerEntero("ID del pasajero: ", id)) return false;
        if (op == 2) {
            const Pasajero* p = s.pasajeros.buscar(id);
            if (p) cout << "  " << p->nombre << " | " << p->origen << " -> " << p->destino << "\n";
            else cout << "  [!] No existe un pasajero con ese ID.\n";
        } else if (op == 4) {
            if (s.pasajeros.eliminar(id)) {
                s.arbol.eliminar(id);                 // mantener el indice sincronizado
                cout << "  [OK] Pasajero eliminado.\n";
                s.historial.apilar("Pasajero eliminado: ID " + to_string(id));
            } else cout << "  [!] El pasajero no existe. No se modifico nada.\n";                     // CP-02
        } else {   // 1 = registrar, 3 = actualizar: piden los mismos datos
            if (op == 1 && s.pasajeros.buscar(id)) { cout << "  [!] ID duplicado.\n"; continue; }
            if (op == 3 && !s.pasajeros.buscar(id)) { cout << "  [!] No existe ese pasajero.\n"; continue; }
            Pasajero p; p.id = id;
            if (!leerTexto("Nombre: ", p.nombre) || !leerTexto("Ciudad origen: ", p.origen) ||
                !leerTexto("Ciudad destino: ", p.destino)) return false;
            if (op == 1) {
                s.pasajeros.insertar(p);
                s.arbol.insertar(p.id, p.nombre);
                cout << "  [OK] Pasajero registrado.\n";
                s.historial.apilar("Pasajero registrado: " + p.nombre);
            } else {
                s.pasajeros.actualizar(id, p.nombre, p.origen, p.destino);
                s.arbol.actualizarNombre(id, p.nombre);
                cout << "  [OK] Pasajero actualizado.\n";
                s.historial.apilar("Pasajero actualizado: ID " + to_string(id));
            }
        }
    }
}

static bool menuCola(Sistema& s) {
    while (true) {
        cout << "\n--- Cola de solicitudes ---\n"
                "  1. Encolar solicitud\n  2. Atender siguiente\n  3. Ver frente\n  4. Mostrar cola\n  0. Volver\n";
        int op; if (!leerEnteroEnRango("Opcion: ", 0, 4, op)) return false;
        if (op == 0) return true;
        Solicitud sol;
        if (op == 1) {
            int id; if (!leerEntero("ID del pasajero: ", id)) return false;
            const Pasajero* p = s.pasajeros.buscar(id);
            if (!p) { cout << "  [!] El pasajero no existe.\n"; continue; }
            // La solicitud solo es valida si origen y destino existen en el grafo.
            if (!s.grafo.buscarCiudad(p->origen) || !s.grafo.buscarCiudad(p->destino)) {
                cout << "  [!] Origen o destino del pasajero no estan registrados como ciudades.\n"; continue;
            }
            sol.idPasajero = p->id; sol.nombre = p->nombre; sol.origen = p->origen; sol.destino = p->destino;
            s.cola.encolar(sol);
            cout << "  [OK] Solicitud encolada.\n";
            s.historial.apilar("Solicitud encolada: " + p->nombre);
        } else if (op == 2) {
            if (s.cola.atender(sol)) {
                cout << "  [OK] Atendida: " << sol.nombre << " (" << sol.origen << " -> " << sol.destino << ")\n";
                s.historial.apilar("Solicitud atendida: " + sol.nombre);
            } else cout << "  [!] La cola esta vacia. No hay nada que atender.\n";                  // CP-03
        } else if (op == 3) {
            if (s.cola.verFrente(sol)) cout << "  Frente: " << sol.nombre << " (" << sol.origen << " -> " << sol.destino << ")\n";
            else cout << "  [!] La cola esta vacia.\n";
        } else s.cola.mostrar();
    }
}

static bool menuHistorial(Sistema& s) {
    while (true) {
        cout << "\n--- Pila de historial ---\n"
                "  1. Ver ultimo evento\n  2. Retirar ultimo evento\n  3. Mostrar historial\n  0. Volver\n";
        int op; if (!leerEnteroEnRango("Opcion: ", 0, 3, op)) return false;
        if (op == 0) return true;
        string e;
        if (op == 1) { if (s.historial.verTope(e)) cout << "  Ultimo: " << e << "\n"; else cout << "  [!] Historial vacio.\n"; }
        else if (op == 2) { if (s.historial.desapilar(e)) cout << "  Retirado: " << e << "\n"; else cout << "  [!] Historial vacio.\n"; }
        else s.historial.mostrar();
    }
}

static bool menuArbol(Sistema& s) {
    while (true) {
        cout << "\n--- Arbol de busquedas (indice de pasajeros por ID) ---\n"
                "  1. Insertar ID en el indice\n  2. Buscar ID\n  3. Eliminar ID del indice\n"
                "  4. Recorrido inorden\n  5. Recorrido preorden\n  6. Recorrido postorden\n"
                "  7. Altura y cantidad de nodos\n  0. Volver\n";
        int op; if (!leerEnteroEnRango("Opcion: ", 0, 7, op)) return false;
        if (op == 0) return true;
        if (op == 4) { s.arbol.inorden(); continue; }
        if (op == 5) { s.arbol.preorden(); continue; }
        if (op == 6) { s.arbol.postorden(); continue; }
        if (op == 7) {
            cout << "  Altura: " << s.arbol.altura() << " niveles | Nodos: " << s.arbol.total() << "\n";
            continue;
        }
        int id; if (!leerEntero("ID: ", id)) return false;
        if (op == 1) {
            const Pasajero* p = s.pasajeros.buscar(id);   // solo se indexan pasajeros que existen
            if (!p) cout << "  [!] No existe un pasajero con ese ID; no se puede indexar.\n";
            else if (s.arbol.insertar(id, p->nombre)) cout << "  [OK] ID insertado en el arbol.\n";
            else cout << "  [!] Ese ID ya esta en el arbol.\n";
        } else if (op == 2) {
            const NodoABB* n = s.arbol.buscar(id);
            if (n) cout << "  [OK] Encontrado: ID " << n->id << " - " << n->nombre << "\n";
            else cout << "  [!] El ID " << id << " no esta en el arbol.\n";
        } else {
            if (s.arbol.eliminar(id)) cout << "  [OK] ID eliminado del arbol.\n";
            else cout << "  [!] El ID " << id << " no esta en el arbol. No se modifico nada.\n";
        }
    }
}

static bool menuRutas(Sistema& s) {
    while (true) {
        cout << "\n--- Rutas y algoritmos ---\n"
                "  1. Recorrido BFS (por niveles)\n  2. Recorrido DFS\n"
                "  3. Componentes conexas\n  4. Camino minimo (Dijkstra)\n  0. Volver\n";
        int op; if (!leerEnteroEnRango("Opcion: ", 0, 4, op)) return false;
        if (op == 0) return true;
        if (op == 3) { componentesConexas(s.grafo); continue; }
        string a;
        if (!leerTexto("Ciudad origen: ", a)) return false;
        if (op == 1) {
            if (!recorridoBFS(s.grafo, a)) cout << "  [!] La ciudad no existe.\n";
        } else if (op == 2) {
            if (!recorridoDFS(s.grafo, a)) cout << "  [!] La ciudad no existe.\n";
        } else {
            string b;
            if (!leerTexto("Ciudad destino: ", b)) return false;
            switch (caminoMinimo(s.grafo, a, b)) {
                case RUTA_OK:              s.historial.apilar("Ruta calculada: " + a + " -> " + b); break;
                case RUTA_CIUDAD_NO_EXISTE: cout << "  [!] Una de las ciudades no existe.\n"; break;
                case RUTA_SIN_CAMINO:       cout << "  [!] No existe camino entre " << a << " y " << b << ".\n"; break;
            }
        }
    }
}

static void tablaBigO() {
    cout << "\n  ANALISIS BIG O (n = pasajeros, V = ciudades, E = conexiones, h = altura ABB)\n"
            "  Lista pasajeros  insertar O(n) | buscar O(n) | actualizar O(n) | eliminar O(n)\n"
            "  Cola             encolar O(1)  | atender O(1) | ver frente O(1)\n"
            "  Pila             apilar O(1)   | desapilar O(1)\n"
            "  ABB              insertar/buscar/eliminar O(h): promedio O(log n), peor caso O(n)\n"
            "  Grafo            agregar ciudad O(V) | agregar conexion O(V + grado) | vecinos O(V + grado)\n"
            "                   eliminar ciudad O(V + E)\n"
            "  BFS / DFS / componentes  O(V + E)\n"
            "  Dijkstra (arreglos)      O(V^2 + E)\n"
            "  Guardar O(V + E + n) | Cargar O(V^2 + E*(V+grado) + n^2) por las busquedas de duplicados\n";
}

static bool menuReportes(Sistema& s) {
    while (true) {
        cout << "\n--- Reportes ---\n"
                "  1. Resumen general\n  2. Grado y distancia por ciudad\n  3. Analisis Big O\n  0. Volver\n";
        int op; if (!leerEnteroEnRango("Opcion: ", 0, 3, op)) return false;
        if (op == 0) return true;
        if (op == 1) {
            cout << "  Ciudades:     " << s.grafo.totalCiudades() << "\n"
                 << "  Conexiones:   " << s.grafo.totalConexiones() << "\n"
                 << "  Pasajeros:    " << s.pasajeros.total() << "\n"
                 << "  En cola:      " << s.cola.total() << "\n"
                 << "  Eventos:      " << s.historial.total() << "\n"
                 << "  Nodos ABB:    " << s.arbol.total() << " (altura " << s.arbol.altura() << ")\n";
        } else if (op == 2) {
            if (s.grafo.totalCiudades() == 0) { cout << "  (no hay ciudades)\n"; continue; }
            const NodoCiudad* mejor = nullptr; int mejorGrado = -1;
            for (NodoCiudad* c = s.grafo.primera(); c != nullptr; c = c->sig) {
                int grado = 0, km = 0;                       // grado = numero de vecinos
                for (Arista* a = c->aristas; a != nullptr; a = a->sig) { grado++; km += a->distancia; }
                cout << "  " << c->nombre << ": grado " << grado << ", suma de distancias " << km << " km\n";
                if (grado > mejorGrado) { mejorGrado = grado; mejor = c; }
            }
            cout << "  Ciudad mas conectada: " << mejor->nombre << " (" << mejorGrado << " conexiones)\n";
        } else tablaBigO();
    }
}

static bool menuArchivos(Sistema& s) {
    while (true) {
        cout << "\n--- Guardar / cargar ---\n  1. Guardar datos\n  2. Cargar datos\n  0. Volver\n";
        int op; if (!leerEnteroEnRango("Opcion: ", 0, 2, op)) return false;
        if (op == 0) return true;
        string ruta, error;
        if (!leerTexto("Nombre del archivo: ", ruta)) return false;
        if (op == 1) {
            if (guardarDatos(ruta, s.grafo, s.pasajeros, error)) {
                cout << "  [OK] Datos guardados en '" << ruta << "'.\n";
                s.historial.apilar("Datos guardados en " + ruta);
            } else cout << "  [!] " << error << "\n";
        } else {
            if (cargarDatos(ruta, s.grafo, s.pasajeros, s.arbol, error)) {
                s.cola.vaciar();     // las solicitudes pendientes no se guardan: se reinician
                cout << "  [OK] Datos cargados desde '" << ruta << "' (la cola de solicitudes se reinicio).\n";
                s.historial.apilar("Datos cargados desde " + ruta);
            } else cout << "  [!] " << error << " No se modifico nada en memoria.\n";
        }
    }
}

static void mostrarMenuPrincipal() {
    cout << "\n=========== SITRAC - MENU PRINCIPAL ===========\n"
            "  1. Gestion de ciudades\n  2. Gestion de conexiones\n  3. Gestion de pasajeros\n"
            "  4. Cola de solicitudes\n  5. Pila de historial\n  6. Arbol de busquedas\n"
            "  7. Rutas y algoritmos\n  8. Reportes\n  9. Guardar / cargar\n  0. Salir\n";
}

void ejecutarMenuPrincipal() {
    Sistema sistema;                 // todas las estructuras viven aqui; se liberan solas al salir
    bool seguir = true;
    while (seguir) {
        mostrarMenuPrincipal();
        int op;
        if (!leerEnteroEnRango("Opcion: ", 0, 9, op)) { cout << "\nEntrada cerrada. Saliendo...\n"; return; }
        switch (op) {
            case OP_CIUDADES:   seguir = menuCiudades(sistema);   break;
            case OP_CONEXIONES: seguir = menuConexiones(sistema); break;
            case OP_PASAJEROS:  seguir = menuPasajeros(sistema);  break;
            case OP_COLA:       seguir = menuCola(sistema);       break;
            case OP_HISTORIAL:  seguir = menuHistorial(sistema);  break;
            case OP_ARBOL:      seguir = menuArbol(sistema);      break;
            case OP_RUTAS:      seguir = menuRutas(sistema);      break;
            case OP_REPORTES:   seguir = menuReportes(sistema);   break;
            case OP_ARCHIVOS:   seguir = menuArchivos(sistema);   break;
            case OP_SALIR:      cout << "\nHasta luego.\n"; seguir = false; break;
        }
    }
}
