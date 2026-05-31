#include "PiezaLider.h"
#include <iostream>

PiezaLider::PiezaLider(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                      // nombre    
        b,                                      // bando
        (b == LUZ ? 110.0f : 60.0f),            // vida
        (b == LUZ ? 40.0f : 25.0f),             // fuerza
        1.5f,                                   // velAtaque
        1.8f,                                   // intervaloAtaque
        3,                                      // radioMovimiento
        fIni,
        cIni,
        TELEPORT                                // TELEPORT
    ) {}

void PiezaLider::atacar(Pieza* enemigo) {
    // Vaciado intencionadamente para la Arena manual.
}

void PiezaLider::dibujar() {
    std::cout << "Dibujando pieza lider: " << nombre << std::endl;
}