#include "PiezaRapida.h"
#include <iostream>

PiezaRapida::PiezaRapida(int fIni, int cIni, TipoMovimiento t) 
    : Pieza(110.0f, 22.0f, 0.7f, 1.0f, 3, fIni, cIni, t) {
    // Constructor PiezaRapida
        //vida = 110.0f
        //fuerza = 22.0f
        //velAtaque = 0.7f
        //intervaloAtaque = 1.0f
        //radioMovimiento = 3
        //Fila inicial todavía sin especificar 
        //Columna inicial todavía sin especificar 
        // Tipo de movimiento todavía sin especificar 
}

void PiezaRapida::atacar() {
    std::cout << "Ataque a distancia fuerte pero lento realizado" << std::endl;
    //Por ahora solo se anuncia por pantalla como va a realizarse su ataque 
}