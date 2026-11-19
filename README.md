# SITRAC - Sistema Inteligente de Transporte y Gestión de Rutas
Estructura de Datos · USTA · C++17 · Todas las estructuras centrales implementadas desde cero (sin STL de contenedores).

**Integrantes:** _(Brity jharetzy cardozo-Oscar erwin bolaños)_

## Compilación y ejecución
```
make          # compila con g++ -std=c++17 -Wall -Wextra
./sitrac      # ejecuta el menú
make test     # corre las 28 pruebas automáticas (resultado en pruebas_log.txt)
```
Sin `make` (Windows): `g++ -std=c++17 -o sitrac src/*.cpp`

## Estructura del proyecto
| Archivo | Responsabilidad |
|---|---|
| `main.cpp` | Punto de entrada (sin lógica) |
| `menus.cpp` | Menú principal y 9 submenús, conectados a las estructuras |
| `entrada.cpp` | Lectura segura de números y textos (nunca cierra ni cuelga el programa) |
| `grafo.cpp` | Grafo no dirigido con lista de adyacencia propia |
| `pasajeros.cpp` | Lista enlazada de pasajeros |
| `cola.cpp` | Cola enlazada de solicitudes (FIFO) |
| `pila.cpp` | Pila enlazada de historial (LIFO) |
| `arbol.cpp` | ABB indexado por ID de pasajero |
| `rutas.cpp` | BFS, DFS, componentes conexas y Dijkstra |
| `archivo.cpp` | Guardar / cargar con validación de integridad |

## Diagrama modular
```
main -> menus -> entrada            (validación de toda entrada)
          |-> grafo  <- rutas       (BFS, DFS, Dijkstra leen el grafo)
          |-> pasajeros + arbol     (lista = fuente de verdad; ABB = índice por ID)
          |-> cola                  (valida pasajero y ciudades antes de encolar)
          |-> pila                  (historial de eventos)
          |-> archivo               (guarda/carga grafo + pasajeros; reconstruye el ABB)
```

## Diseño: operación → estructura → justificación
| Operación | Estructura | Justificación |
|---|---|---|
| CRUD de pasajeros | Lista enlazada propia | Altas y bajas sin reubicar elementos; sin tamaño fijo |
| Búsqueda de pasajero por ID ordenada | ABB | Recorrido inorden entrega IDs ordenados; búsqueda O(h) |
| Ciudades y conexiones | Grafo con lista de adyacencia | Grafo disperso: memoria O(V+E) y vecinos directos |
| Solicitudes de viaje | Cola enlazada (frente + fin) | Atención FIFO, encolar/atender O(1) |
| Historial / deshacer | Pila enlazada | Último evento primero (LIFO), O(1) |
| Alcance por niveles | BFS con cola | Visita por distancia en saltos |
| Exploración / componentes | DFS recursivo | La pila de llamadas actúa como pila |
| Camino de menor distancia | Dijkstra con arreglos | Pesos no negativos; O(V²) simple y fácil de justificar |

## Análisis Big O
n = pasajeros, V = ciudades, E = conexiones, h = altura del ABB.
| Operación | Complejidad |
|---|---|
| Lista: insertar / buscar / actualizar / eliminar | O(n) |
| Cola: encolar / atender | O(1) |
| Pila: apilar / desapilar | O(1) |
| ABB: insertar / buscar / eliminar | O(h): promedio O(log n), peor caso O(n) (IDs en orden) |
| Grafo: agregar ciudad | O(V) (verifica duplicados) |
| Grafo: agregar conexión | O(V + grado) |
| Grafo: eliminar ciudad | O(V + E) |
| BFS / DFS / componentes | O(V + E) (cada ciudad tiene un `indice`, el acceso a visitados es O(1)) |
| Dijkstra | O(V² + E) |

## Decisiones de diseño importantes
- **Entradas seguras:** siempre se lee una línea completa y se valida; nunca `cin >> n`.
- **Integridad del grafo:** al eliminar una ciudad se borran primero las aristas que apuntan a ella.
- **ABB sincronizado:** registrar, actualizar y eliminar pasajeros actualiza el índice.
- **Persistencia en dos fases:** el archivo se valida completo en estructuras temporales; solo si es válido reemplaza la memoria. Se guarda en un `.tmp` y luego se renombra.
- **Neighbors en orden de creación:** las aristas se agregan al final, así BFS/DFS son reproducibles.

## Pruebas
`make test` ejecuta 28 pruebas (ver `pruebas_log.txt`): CP-01..CP-06, ID duplicado, ABB (inorden, preorden, búsqueda inexistente, eliminar nodo con dos hijos), BFS, DFS, Dijkstra (camino, costo, sin camino), componentes, cola FIFO y persistencia (archivo inexistente, corrupto, distancia negativa, guardar/cargar, memoria intacta tras fallo). También se verificó con AddressSanitizer/UBSanitizer sin errores ni fugas.

## Limitaciones
- Solo hay grafo no dirigido; no se guardan las solicitudes de la cola ni el historial.
- Los nombres no pueden contener el carácter `|` (separador del archivo).
- La búsqueda de ciudades por nombre es O(V) (lista enlazada); una tabla hash la mejoraría.
- Dijkstra usa arreglos O(V²); con un montículo sería O((V+E) log V).

## Estado de funcionalidades
| Funcionalidad | Estado | Responsable |
|---|---|---|
| Menú, modularidad y entradas | Terminado | _(britsy)_ |
| Lista de pasajeros (CRUD) | Terminado | _(oscar)_ |
| Cola y pila | Terminado | _(oscar)_ |
| ABB (insertar/buscar/eliminar/recorridos) | Terminado | _(britsy)_ |
| Grafo (CRUD e integridad) | Terminado | _(britsy)_ |
| BFS, DFS, componentes, Dijkstra | Terminado | _(britsy)_ |
| Guardar / cargar | Terminado | _(oscar)_ |
| Reportes y Big O | Terminado | _(oscar)_ |

## Uso de IA (declaración obligatoria)
- **Qué se consultó:** se usó Claude (Anthropic) para analisar y documentar las funcionalidades del codigo incluyendo un analisis de errores que los programadores no lograban encontrar.
- **Qué revisamos y corregimos:** _(errores de copilacion, malas definiciones en los comandos mala organizacion de los archivos y copilacion.)_
