#include "PiezaTanque.h"
#include <iostream>

PiezaTanque::PiezaTanque(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,    // nombre
        b,                                      // bando
        (b == LUZ ? 200.0f : 220.0f),          // vida
        (b == LUZ ? 35.0f : 40.0f),            // fuerza
        (b == LUZ ? 0.4f : 0.3f),              // velAtaque
        (b == LUZ ? 2.0f : 2.5f),              // intervaloAtaque
        2,                                      // radioMovimiento
        fIni,                                   // fila inicial
        cIni,                                   // columna inicial
        GROUND                                  // tipo de movimiento
    ) {
    // Constructor de PiezaTanque:
    // Si es del bando LUZ, se crea como Pomelo.
    // Si es del bando OSCURIDAD, se crea como All-Star.
}

void PiezaTanque::atacar(Pieza* enemigo) {
    std::cout << nombre << " realiza un ataque de tanque." << std::endl;
}
void PiezaTanque::dibujar() {
    std::cout << "Dibujando pieza tanque: " << nombre << std::endl;
}