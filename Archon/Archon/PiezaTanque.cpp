#include "PiezaTanque.h"
#include <iostream>

PiezaTanque::PiezaTanque(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                      // nombre
        b,                                      // bando
        150.0f,                                 // vida 
        40.0f,                                  // fuerza 
        0.6f,                                   // velAtaque 
        2.5f,                                   // intervaloAtaque 
        2,                                      // radioMovimiento 
        fIni, cIni, GROUND
    ) {}

void PiezaTanque::atacar(Pieza* enemigo) {
    // Vaciado para usar la Arena manual
}

void PiezaTanque::dibujar() {
    std::cout << "Dibujando pieza tanque: " << nombre << std::endl;
}