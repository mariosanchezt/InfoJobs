#include <iostream>
#include "Pieza.h"
#include "PiezaMelee.h"
#include "PiezaDistancia.h"
#include "PiezaRapida.h"
#include "PiezaTanque.h"
#include "PiezaVoladora.h"
#include "Tablero.h"
#include "Arena.h"
#include "Juego.h"

int main() {

    // 1. POSICIONES DEL TABLERO

    int filaMeleeLuz1 = 0, colMeleeLuz1 = 1;
    int filaMeleeOsc1 = 0, colMeleeOsc1 = 7;

    int filaTanqueLuz = 0, colTanqueLuz = 0;
    int filaDistanciaLuz = 1, colDistanciaLuz = 0;
    int filaVoladoraLuz = 2, colVoladoraLuz = 0;
    int filaRapidaLuz = 3, colRapidaLuz = 0;

    int filaTanqueOsc = 0, colTanqueOsc = 8;
    int filaDistanciaOsc = 1, colDistanciaOsc = 8;
    int filaVoladoraOsc = 2, colVoladoraOsc = 8;
    int filaRapidaOsc = 3, colRapidaOsc = 8;


    // 2. CREACION DE PIEZAS

    // Bando LUZ - Plantas
    PiezaTanque tanqueLuz("Pomelo", LUZ, filaTanqueLuz, colTanqueLuz, GROUND);
    PiezaDistancia distanciaLuz("Lanzaguisantes", LUZ, filaDistanciaLuz, colDistanciaLuz, GROUND);
    PiezaVoladora voladoraLuz("Mazorca", LUZ, filaVoladoraLuz, colVoladoraLuz, FLYING);
    PiezaRapida rapidaLuz("Girasol", LUZ, filaRapidaLuz, colRapidaLuz, GROUND);
    PiezaMelee meleeLuz("Carnivora", LUZ, filaMeleeLuz1, colMeleeLuz1, GROUND);

    // Bando OSCURIDAD - Zombies
    PiezaTanque tanqueOsc("All-Star", OSCURIDAD, filaTanqueOsc, colTanqueOsc, GROUND);
    PiezaDistancia distanciaOsc("Soldado", OSCURIDAD, filaDistanciaOsc, colDistanciaOsc, GROUND);
    PiezaVoladora voladoraOsc("Ingeniero", OSCURIDAD, filaVoladoraOsc, colVoladoraOsc, FLYING);
    PiezaRapida rapidaOsc("Zombidito", OSCURIDAD, filaRapidaOsc, colRapidaOsc, GROUND);
    PiezaMelee meleeOsc("Supercerebroz", OSCURIDAD, filaMeleeOsc1, colMeleeOsc1, GROUND);


    // 3. LLAMADAS A DIBUJAR

    tanqueLuz.dibujar();
    distanciaLuz.dibujar();
    voladoraLuz.dibujar();
    rapidaLuz.dibujar();
    meleeLuz.dibujar();

    tanqueOsc.dibujar();
    distanciaOsc.dibujar();
    voladoraOsc.dibujar();
    rapidaOsc.dibujar();
    meleeOsc.dibujar();


    // 4. ACCESO AL TABLERO

    Tablero tablero;
    tablero.dibujar();

    // Colocacion teorica de algunas piezas
    tablero.colocarPieza(filaTanqueLuz, colTanqueLuz, &tanqueLuz);
    tablero.colocarPieza(filaRapidaLuz, colRapidaLuz, &rapidaLuz);
    tablero.colocarPieza(filaTanqueOsc, colTanqueOsc, &tanqueOsc);
    tablero.colocarPieza(filaRapidaOsc, colRapidaOsc, &rapidaOsc);

    // 5. ACCESO A LA ARENA

    Arena arena;
    arena.iniciarCombate(&tanqueLuz, &tanqueOsc);
    //Se ha llamado a pelear de forma teórica a las piezas de tipo tanque de ambos bandos 
    arena.dibujar();

    // 6. ACCESO AL JUEGO

    Juego juego;
    juego.inicializarPartida();
    juego.dibujar();

    return 0;
}