#include "PiezaLider.h"
#include <iostream>

PiezaLider::PiezaLider(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                      // nombre
        b,                                      // bando
        (b == LUZ ? 120.0f : 140.0f),           // vida
        (b == LUZ ? 20.0f : 25.0f),             // fuerza
        (b == LUZ ? 0.5f : 0.4f),               // velAtaque
        (b == LUZ ? 1.0f : 1.2f),               // intervaloAtaque
        4,                                      // radioMovimiento
        fIni,                                   // fila inicial
        cIni,                                   // columna inicial
        TELEPORT                                // tipo de movimiento
    ) {
    hechizosDisponibles = 7;

    // Constructor de PiezaLider:
    // Si es del bando LUZ, se crea como Dave el Loco.
    // Si es del bando OSCURIDAD, se crea como Dr. Zomboss.
}

void PiezaLider::atacar(Pieza* enemigo) {
    if (enemigo != nullptr) {
        std::cout << nombre << " realiza un ataque especial contra "
            << enemigo->getNombre() << std::endl;

        enemigo->vida -= this->fuerza;

        if (enemigo->vida < 0) {
            enemigo->vida = 0;
        }

        std::cout << "Vida restante de " << enemigo->getNombre()
            << ": " << enemigo->vida << std::endl;
    }
}

void PiezaLider::dibujar() {
    std::cout << "Dibujando pieza lider: " << nombre << std::endl;
}

void PiezaLider::lanzarHechizo() {
    if (hechizosDisponibles > 0) {
        hechizosDisponibles--;

        std::cout << nombre << " lanza un hechizo. Hechizos restantes: "
            << hechizosDisponibles << std::endl;
    }
    else {
        std::cout << nombre << " no tiene hechizos disponibles." << std::endl;
    }
}