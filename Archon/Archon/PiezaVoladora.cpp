#include "PiezaVoladora.h"
#include <iostream>

PiezaVoladora::PiezaVoladora(int fIni, int cIni)
    : Pieza(120.0f, 20.0f, 1.2f, 0.8f, 3, fIni, cIni, FLYING) {
    //Constructor PiezaVoladora (Phoenix)
        //vida = 120.0f
        //fuerza = 20.0f
        //velAtaque = 1.2f
        //intervaloAtaque = 0.8f
        //radioMovimiento = 3
        //Fila inicial todavia sin especificar
        //Columna inicial todavia sin especificar
        //Tipo de movimiento siempre FLYING
    modoBolaDeFuego = false;
}

void PiezaVoladora::atacar() {
    if (modoBolaDeFuego) {
        std::cout << "Ataque en modo bola de fuego!" << std::endl;
    }
    else {
        std::cout << "Ataque de fuego realizado" << std::endl;
    }
    //Por ahora solo se anuncia por pantalla como va a realizarse su ataque
}

void PiezaVoladora::activarBolaDeFuego() {
    modoBolaDeFuego = true;
    fuerza *= 1.5f; //sube el dano al transformarse
    std::cout << "Phoenix se transforma en bola de fuego" << std::endl;
}

void PiezaVoladora::desactivarBolaDeFuego() {
    modoBolaDeFuego = false;
    fuerza /= 1.5f; //vuelve a la normalidad
    std::cout << "Phoenix vuelve a su forma normal" << std::endl;
}
