#include "PiezaMelee.h"
#include <iostream>

PiezaMelee::PiezaMelee(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                      // nombre
        b,                                      // bando
        40.0f,                                  // vida
        10.0f,                                  // fuerza 
        1.0f,                                   // velAtaque 
        0.4f,                                   // intervaloAtaque
        3,                                      // radioMovimiento en el tablero
        fIni, cIni, GROUND
    ) {}

void PiezaMelee::atacar(Pieza* enemigo) {
    // Vaciamos esta función intencionadamente.
    // El combate en tiempo real (ondas expansivas, cooldowns y daño) 
    // ahora lo gestiona en exclusiva la clase Arena.
}

void PiezaMelee::dibujar() {
    std::cout << "Dibujando pieza melee: " << nombre << std::endl;
}