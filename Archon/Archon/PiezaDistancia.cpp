#include "PiezaDistancia.h"
#include <iostream>

PiezaDistancia::PiezaDistancia(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                      // nombre
        b,                                      // bando
        100.0f,                                 // vida
        (b == LUZ ? 25.0f : 22.0f),             // fuerza
        (b == LUZ ? 0.8f : 0.9f),               // velAtaque
        (b == LUZ ? 0.8f : 0.6f),               // intervaloAtaque
        4,                                      // radioMovimiento
        fIni,                                   // fila inicial
        cIni,                                   // columna inicial
        GROUND                                  // tipo de movimiento
    )
{
    alcance = (b == LUZ ? 5.0f : 7.0f);
}

void PiezaDistancia::atacar(Pieza* enemigo) {
    if (enemigo != nullptr) {
        std::cout << nombre << " dispara un proyectil de alcance " << alcance
            << " contra " << enemigo->getNombre() << std::endl;

        enemigo->vida -= this->fuerza;

        if (enemigo->vida < 0) {
            enemigo->vida = 0;
        }

        std::cout << "Vida restante de " << enemigo->getNombre()
            << ": " << enemigo->vida << std::endl;
    }
}

void PiezaDistancia::dibujar() {
    std::cout << "Dibujando pieza a distancia: " << nombre << std::endl;
}