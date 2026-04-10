#include "Juego.h"
#include "Tablero.h"
#include "Arena.h"
#include "Pieza.h"
#include "PiezaMelee.h"
#include "PiezaDistancia.h"
#include "PiezaRapida.h"
#include "PiezaTanque.h"
#include "PiezaVoladora.h"
#include <iostream>

Juego::Juego() {
    tablero = new Tablero();
    arena = new Arena();
    turnoActual = LUZ;
    hechizosRestantesLuz = 7;
    hechizosRestantesOscuridad = 7;
}

void Juego::inicializarPartida() {
    PiezaMelee* guerrero = new PiezaMelee(0, 0, GROUND);
    tablero->colocarPieza(0, 0, guerrero);

    std::cout << "Partida inicializada. Turno de la Luz." << std::endl;
}

void Juego::cambiarTurno() {
    turnoActual = (turnoActual == LUZ) ? OSCURIDAD : LUZ;
    std::cout << "Cambio de turno. Ahora le toca a: "
        << (turnoActual == LUZ ? "Luz" : "Oscuridad") << std::endl;
}

void Juego::moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino) {

}


void Juego::lanzarHechizo(int idHechizo, Pieza* objetivo, int fDest, int cDest) {
    // 1. COMPROBACIÓN
    bool* listaUsados = (turnoActual == LUZ) ? hechizosUsadosLuz : hechizosUsadosOscuridad;

    // Ajustamos el ID para que coincida con el array (ID 1 -> Posición 0)
    int indice = idHechizo - 1;

    if (indice < 0 || indice >= 7 || listaUsados[indice] == true) {
        std::cout << "Hechizo no valido o ya agotado." << std::endl;
        return;
    }

    // 2. EJECUCIÓN
    switch (idHechizo) {
    case 1: // HEAL
        if (objetivo != nullptr) {
            objetivo->vida = 100.0f;
            std::cout << "Hechizo de Curacion lanzado." << std::endl;
        }
        break;
    case 2: // TELEPORT
        if (objetivo != nullptr) {
            tablero->colocarPieza(fDest, cDest, objetivo);
            std::cout << "Teletransporte realizado." << std::endl;
        }
        break;
    default:
        std::cout << "Este hechizo aun no esta implementado." << std::endl;
        return;
    }

    // 3. GASTO DE RECURSOS
    listaUsados[indice] = true; // Marcamos este hechizo concreto como usado

    // Para el contador total, restamos de la variable de clase directamente
    if (turnoActual == LUZ) {
        hechizosRestantesLuz--;
        std::cout << "Le quedan " << hechizosRestantesLuz << " hechizos a la Luz." << std::endl;
    }
    else {
        hechizosRestantesOscuridad--;
        std::cout << "Le quedan " << hechizosRestantesOscuridad << " hechizos a la Oscuridad." << std::endl;
    }

    cambiarTurno();
}

Juego::~Juego() {
    delete tablero;
    delete arena;
}