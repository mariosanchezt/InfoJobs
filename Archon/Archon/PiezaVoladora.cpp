#include "PiezaVoladora.h"
#include <iostream>

PiezaVoladora::PiezaVoladora(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                      // nombre
        b,                                      // bando
        (b == LUZ ? 110.0f : 120.0f),           // vida
        (b == LUZ ? 30.0f : 28.0f),             // fuerza
        (b == LUZ ? 0.7f : 0.6f),               // velAtaque
        (b == LUZ ? 1.0f : 1.0f),               // intervaloAtaque (PENDIENTE confirmar del Excel)
        5,                                      // radioMovimiento
        fIni,                                   // fila inicial
        cIni,                                   // columna inicial
        FLYING                                  // tipo de movimiento siempre FLYING
    ) {
    // Constructor de PiezaVoladora:
    // Si es del bando LUZ, se crea como Mazorca (Kernel Corn).
    // Si es del bando OSCURIDAD, se crea como Ingeniero (Dron).
    modoBolaDeFuego = false;
}

void PiezaVoladora::atacar(Pieza* enemigo) {
    if (enemigo != nullptr) {
        if (modoBolaDeFuego) {
            std::cout << nombre << " ataca en modo bola de fuego contra " << enemigo->getNombre() << std::endl;
        }
        else {
            std::cout << nombre << " realiza un ataque aereo contra " << enemigo->getNombre() << std::endl;
        }

        // Logica de dano: restamos tu fuerza a su vida
        enemigo->vida -= this->fuerza;

        std::cout << "Vida restante de " << enemigo->getNombre() << ": " << enemigo->vida << std::endl;
    }
}

void PiezaVoladora::activarBolaDeFuego() {
    modoBolaDeFuego = true;
    fuerza *= 1.5f; //sube el dano al transformarse
    std::cout << nombre << " se transforma en bola de fuego" << std::endl;
}

void PiezaVoladora::desactivarBolaDeFuego() {
    modoBolaDeFuego = false;
    fuerza /= 1.5f; //vuelve a la normalidad
    std::cout << nombre << " vuelve a su forma normal" << std::endl;
}

void PiezaVoladora::dibujar() {
    std::cout << "Dibujando pieza voladora: " << nombre << std::endl;
}
