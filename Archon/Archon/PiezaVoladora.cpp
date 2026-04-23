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
            std::cout << nombre << "arrolla en modo BOLA DE FUEGO" << std::endl;
        }
        else {
            std::cout << nombre << "lanza un ataque de fuego desde el aire" << std::endl;
        }
        //Resta la vida real
        enemigo->vida -= this->fuerza;
        if (enemigo->vida < 0)enemigo->vida = 0;//Evita que la vida pueda ser negativa

        if (enemigo->vida == 0) {
            std::cout << enemigo->getNombre() << " ha sido calcinado." << std::endl;
        }
        else {
            std::cout << enemigo->getNombre() << " se está quemando. Vida restante: " << enemigo->vida << std::endl;
        }
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
