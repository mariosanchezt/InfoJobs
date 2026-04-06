#include "PiezaTanque.h"
#include <iostream>

PiezaTanque::PiezaTanque(int fIni, int cIni, TipoMovimiento t)
    : Pieza(180.0f, 25.0f, 0.6f, 1.2f, 1, fIni, cIni, t) {
    // Constructor PiezaTanque
        //vida = 180.0f
        //fuerza = 25.0f
        //velAtaque = 0.6f
        //intervaloAtaque = 1.2f
        //radioMovimiento = 1
        //Fila inicial todavía sin especificar 
        //Columna inicial todavía sin especificar 
        //Tipo de movimiento todavía sin especificar 
}

void PiezaTanque::atacar() {
    std::cout << "Ataque cuerpo a cuerpo pesado realizado" << std::endl;
    //Por ahora solo se anuncia por pantalla como va a realizarse su ataque 
}