#!/bin/bash
# Pruebas reproducibles de SITRAC. Uso: make test   (o: bash pruebas.sh)
# Cada prueba alimenta el programa por teclado simulado (stdin) y busca el texto esperado.
BIN="$(pwd)/sitrac"
TMP=$(mktemp -d)              # carpeta temporal: los archivos de datos no ensucian el proyecto
pass=0; fail=0

# t "nombre" "entrada" "texto esperado"
t() {
  out=$(cd "$TMP" && printf "$2" | "$BIN" 2>&1)
  if echo "$out" | grep -qF -- "$3"; then echo "PASS  $1"; pass=$((pass+1))
  else echo "FAIL  $1   (se esperaba: $3)"; fail=$((fail+1)); fi
}

# tn "nombre" "entrada" "texto que NO debe aparecer"
tn() {
  out=$(cd "$TMP" && printf "$2" | "$BIN" 2>&1)
  if echo "$out" | grep -qF -- "$3"; then echo "FAIL  $1   (no debia aparecer: $3)"; fail=$((fail+1))
  else echo "PASS  $1"; pass=$((pass+1)); fi
}

# Bloques de entrada reutilizables (cada \n equivale a pulsar Enter)
CIUDADES='1\n1\nA\n1\nB\n1\nC\n1\nD\n0\n'                                   # ciudades A,B,C,D
RUTAS='2\n1\nA\nB\n1\n1\nA\nC\n4\n1\nB\nC\n2\n1\nC\nD\n1\n0\n'               # A-B 1, A-C 4, B-C 2, C-D 1
PAS='3\n1\n5\nAna\nA\nD\n1\n2\nBeto\nA\nD\n1\n8\nCami\nA\nD\n0\n'            # pasajeros 5,2,8

t "CP-01 ciudad duplicada se rechaza"        '1\n1\nBogota\n1\nBogota\n0\n0\n'   "ya existe"
t "CP-02 eliminar pasajero inexistente"      '3\n4\n99\n0\n0\n'                  "El pasajero no existe"
t "CP-03 atender cola vacia"                 '4\n2\n0\n0\n'                      "La cola esta vacia"
t "CP-04 distancia negativa"                 "${CIUDADES}2\n1\nA\nB\n-5\n0\n0\n" "mayor que 0"
t  "CP-05 eliminar ciudad conectada (vecina sigue viva)" "${CIUDADES}${RUTAS}1\n2\nB\n0\n2\n3\nA\n0\n0\n" "- C (4 km)"
tn "CP-05 sin referencias colgantes a la ciudad borrada" "${CIUDADES}${RUTAS}1\n2\nB\n0\n2\n3\nA\n0\n0\n" "- B ("
t "CP-05b vecinos ya no incluyen la ciudad"  "${CIUDADES}2\n1\nA\nB\n1\n0\n1\n2\nB\n0\n2\n3\nA\n0\n0\n" "(sin conexiones)"
t "CP-06 letras donde se pide numero"        'abc\n3\n1\nhola\n0\n0\n'           "no es un numero"
t "Pasajero con ID duplicado"                '3\n1\n1\nAna\nA\nB\n1\n1\nOtro\nA\nB\n0\n0\n' "ID duplicado"
t "ABB inorden ordenado"                     "${PAS}6\n4\n0\n0\n"                "Inorden: 2, 5, 8"
t "ABB preorden refleja la insercion"        "${PAS}6\n5\n0\n0\n"                "Preorden: 5, 2, 8"
t "ABB buscar ID inexistente"                '6\n2\n99\n0\n0\n'                  "no esta en el arbol"
t "ABB eliminar nodo con dos hijos"          "${PAS}6\n3\n5\n4\n0\n0\n"          "Inorden: 2, 8"
t "BFS por niveles"                          "${CIUDADES}${RUTAS}7\n1\nA\n0\n0\n" "4. D (nivel 2)"
t "BFS con ciudad inexistente"               '7\n1\nZZ\n0\n0\n'                  "La ciudad no existe"
t "DFS visita todas"                         "${CIUDADES}${RUTAS}7\n2\nA\n0\n0\n" "4. D"
t "Dijkstra camino minimo"                   "${CIUDADES}${RUTAS}7\n4\nA\nD\n0\n0\n" "A -> B -> C -> D"
t "Dijkstra costo total"                     "${CIUDADES}${RUTAS}7\n4\nA\nD\n0\n0\n" "Costo total: 4 km"
t "Dijkstra sin camino (ciudad aislada)"     "${CIUDADES}1\n1\nE\n0\n${RUTAS}7\n4\nA\nE\n0\n0\n" "No existe camino"
t "Componentes conexas"                      "${CIUDADES}1\n1\nE\n0\n${RUTAS}7\n3\n0\n0\n" "Total de componentes: 2"
t "Cola respeta el orden FIFO"               "1\n1\nA\n1\nB\n0\n3\n1\n1\nAna\nA\nB\n1\n2\nBeto\nA\nB\n0\n4\n1\n1\n1\n2\n2\n0\n0\n" "Atendida: Ana"

# --- Persistencia (varias ejecuciones en la misma carpeta temporal) ---
t "Cargar archivo inexistente"               '9\n2\nno_existe.txt\n0\n0\n'        "No se pudo abrir"
t "Guardar datos"                            "${CIUDADES}${RUTAS}${PAS}9\n1\ndatos.txt\n0\n0\n" "Datos guardados"
t "Cargar restaura ciudades y conexiones"    '9\n2\ndatos.txt\n0\n8\n1\n0\n0\n'     "Conexiones:   4"
t "Cargar restaura el indice ABB"            '9\n2\ndatos.txt\n0\n6\n4\n0\n0\n'     "Inorden: 2, 5, 8"
printf 'SITRAC 1\nCIUDADES 3\nA\nB\n' > "$TMP/corrupto.txt"                      # promete 3 ciudades, trae 2
printf 'SITRAC 1\nCIUDADES 2\nA\nB\nCONEXIONES 1\nA|B|-7\nPASAJEROS 0\nFIN\n' > "$TMP/negativa.txt"
t "Archivo corrupto se rechaza"              '9\n2\ncorrupto.txt\n0\n0\n'        "Archivo corrupto"
t "Archivo con distancia negativa se rechaza" '9\n2\nnegativa.txt\n0\n0\n'       "Archivo corrupto"
t "Archivo corrupto no altera la memoria"    '1\n1\nZ\n0\n9\n2\ncorrupto.txt\n0\n1\n3\n0\n0\n' "  - Z"

rm -rf "$TMP"
echo; echo "Resultado: $pass aprobadas, $fail fallidas (total $((pass+fail)))"
[ "$fail" -eq 0 ]
