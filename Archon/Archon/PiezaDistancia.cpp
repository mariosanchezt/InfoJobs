#include "PiezaDistancia.h"
#include <iostream>

PiezaDistancia::PiezaDistancia(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                      // nombre
        b,                                      // bando
        (b == LUZ ? 40.0f : 60.0f),             // vida
        15.0f,                                  // fuerza
        (b == LUZ ? 1.0f : 0.6f),               // velAtaque
        1.8f,                                   // intervaloAtaque
        3,                                      // radioMovimiento en el tablero
        fIni,
        cIni,
        GROUND                                  // tipo de movimiento
    )
{
    
    alcance = (b == LUZ ? 6.0f : 5.0f);
}

void PiezaDistancia::atacar(Pieza* enemigo) {
   
}

void PiezaDistancia::dibujar() {
    std::cout << "Dibujando pieza a distancia: " << nombre << std::endl;
}