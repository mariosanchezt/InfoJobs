#include "PiezaVoladora.h"
#include <iostream>

PiezaVoladora::PiezaVoladora(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                      // nombre
        b,                                      // bando
        80.0f,                                  // vida
        25.0f,                                  // fuerza
        0.6f,                                   // velAtaque
        1.2f,                                   // intervaloAtaque
        3,                                      // radioMovimiento
        fIni,
        cIni,
        FLYING                                  // FLYING
    ) {}

void PiezaVoladora::atacar(Pieza* enemigo) {
    // Vaciado intencionadamente para la Arena manual.
}

void PiezaVoladora::dibujar() {
    std::cout << "Dibujando pieza voladora: " << nombre << std::endl;
}