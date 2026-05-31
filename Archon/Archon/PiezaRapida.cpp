#include "PiezaRapida.h"
#include <iostream>

PiezaRapida::PiezaRapida(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                      // nombre   
        b,                                      // bando
        60.0f,                                  // vida
        25.0f,                                  // fuerza
        1.5f,                                   // velAtaque 
        (b == LUZ ? 0.8f : 1.2f),               // intervaloAtaque
        4,                                      // radioMovimiento
        fIni,
        cIni,
        GROUND                                  // tipo de movimiento
    ) {}

void PiezaRapida::atacar(Pieza* enemigo) {
    // Vaciado intencionadamente para la Arena manual.
}

void PiezaRapida::dibujar() {
    std::cout << "Dibujando pieza rapida: " << nombre << std::endl;
}