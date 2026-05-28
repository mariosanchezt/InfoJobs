#include "PiezaMelee.h"
#include <iostream>


PiezaMelee::PiezaMelee(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        n,                                      // nombre
        b,                                      // bando
        (b == LUZ ? 150.0f : 130.0f),           // vida
        (b == LUZ ? 45.0f : 50.0f),             // fuerza
        (b == LUZ ? 1.2f : 1.0f),               // velAtaque
        (b == LUZ ? 1.5f : 1.2f),               // intervaloAtaque
        (b == LUZ ? 3 : 4),                     // radioMovimiento
        fIni,                                   // fila inicial
        cIni,                                   // columna inicial
        GROUND                                  // tipo de movimiento
    ) {
    // Constructor PiezaMelee:
    // Si es del bando LUZ, se crea como Carnivora.
    // Si es del bando OSCURIDAD, se crea como Supercerebroz.
}

//Atributo atacar de PiezaMelee
void PiezaMelee::atacar(Pieza* enemigo) {
   if (enemigo != nullptr) {
   std::cout << nombre << " realiza un ataque cuerpo a cuerpo contra " << enemigo->getNombre() << std::endl;

   // Lógica de daño real: restamos tu fuerza a su vida
   enemigo->vida -= this->fuerza;
     
   if (enemigo->vida < 0) enemigo->vida = 0;//Evita que la vida sea negativa
   std::cout << "Vida restante de " << enemigo->getNombre() << ": " << enemigo->vida << std::endl;
   }
}

void PiezaMelee::dibujar() {
    std::cout << "Dibujando pieza melee: " << nombre << std::endl;
}