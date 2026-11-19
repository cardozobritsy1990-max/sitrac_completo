// menus.h  -  Menú principal y submenús de SITRAC
// Cada submenú vive en su propia función para que main() NO tenga lógica.
// En esta primera etapa los submenús solo tienen la opción "Volver"; las
// operaciones reales se agregan módulo por módulo en los pasos siguientes.
#ifndef MENUS_H
#define MENUS_H

// Ciclo del menú principal: se repite hasta que el usuario elige 0 (Salir)
// o hasta que la entrada se cierra.
void ejecutarMenuPrincipal();

#endif // MENUS_H
