#include "PiezaRapida.h"
#include <iostream>

PiezaRapida::PiezaRapida(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                     // nombre
        b,                                     // bando
        (b == LUZ ? 80.0f : 60.0f),            // vida
        (b == LUZ ? 15.0f : 12.0f),            // fuerza
        (b == LUZ ? 0.5f : 0.3f),              // velAtaque
        (b == LUZ ? 0.4f : 0.2f),              // intervaloAtaque
        (b == LUZ ? 6 : 7),                    // radioMovimiento
        fIni,                                  // fila inicial
        cIni,                                  // columna inicial
        GROUND                                 // tipo de movimiento
    ) {
    // Constructor de PiezaRapida:
    // Si es del bando LUZ, se crea como Girasol.
    // Si es del bando OSCURIDAD, se crea como Balloon.
}

void PiezaRapida::atacar(Pieza* enemigo) {
    if (enemigo != nullptr) {
        std::cout << nombre << "ataca a la velocidad del rayo!" << std::endl;
        //Resta de la vida real
        enemigo->vida -= this->fuerza;

        if (enemigo->vida < 0) enemigo->vida = 0;//Para que no salga vida negativa
        std::cout << "Vida restante de " << enemigo->getNombre() << ": " << enemigo->vida << std::endl;
    }
}
void PiezaRapida::dibujar() {
    std::cout << "Dibujando pieza rapida: " << nombre << std::endl;
}