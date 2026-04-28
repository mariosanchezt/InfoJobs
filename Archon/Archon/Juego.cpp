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
    for (int i = 0; i < 7; i++) {
        hechizosUsadosLuz[i] = false;
        hechizosUsadosOscuridad[i] = false;
    }
}

void Juego::inicializarPartida() {
    // MELEE: columna interior, como los peones del Archon original
    for (int i = 0; i < 9; i++) {
        tablero->colocarPieza(i, 1, new PiezaMelee("Carnivora", LUZ, i, 1, GROUND));
        tablero->colocarPieza(i, 7, new PiezaMelee("Supercerebroz", OSCURIDAD, i, 7, GROUND));
    }

    // BANDO LUZ - PLANTAS, columna exterior
    tablero->colocarPieza(0, 0, new PiezaTanque("Pomelo", LUZ, 0, 0, GROUND));
    tablero->colocarPieza(1, 0, new PiezaDistancia("Lanzaguisantes", LUZ, 1, 0, GROUND));
    tablero->colocarPieza(2, 0, new PiezaVoladora("Mazorca", LUZ, 2, 0, FLYING));
    tablero->colocarPieza(3, 0, new PiezaRapida("Girasol", LUZ, 3, 0, GROUND));

    // tablero->colocarPieza(4, 0, new PiezaLider("Dave el Loco", LUZ, 4, 0, TELEPORT));

    tablero->colocarPieza(5, 0, new PiezaRapida("Girasol", LUZ, 5, 0, GROUND));
    tablero->colocarPieza(6, 0, new PiezaVoladora("Mazorca", LUZ, 6, 0, FLYING));
    tablero->colocarPieza(7, 0, new PiezaDistancia("Lanzaguisantes", LUZ, 7, 0, GROUND));
    tablero->colocarPieza(8, 0, new PiezaTanque("Pomelo", LUZ, 8, 0, GROUND));

    // BANDO OSCURIDAD - ZOMBIES, columna exterior
    tablero->colocarPieza(0, 8, new PiezaTanque("All-Star", OSCURIDAD, 0, 8, GROUND));
    tablero->colocarPieza(1, 8, new PiezaDistancia("Soldado", OSCURIDAD, 1, 8, GROUND));
    tablero->colocarPieza(2, 8, new PiezaVoladora("Ingeniero", OSCURIDAD, 2, 8, FLYING));
    tablero->colocarPieza(3, 8, new PiezaRapida("Zombidito", OSCURIDAD, 3, 8, GROUND));

    // tablero->colocarPieza(4, 8, new PiezaLider("Dr. Zombi", OSCURIDAD, 4, 8, TELEPORT));

    tablero->colocarPieza(5, 8, new PiezaRapida("Zombidito", OSCURIDAD, 5, 8, GROUND));
    tablero->colocarPieza(6, 8, new PiezaVoladora("Ingeniero", OSCURIDAD, 6, 8, FLYING));
    tablero->colocarPieza(7, 8, new PiezaDistancia("Soldado", OSCURIDAD, 7, 8, GROUND));
    tablero->colocarPieza(8, 8, new PiezaTanque("All-Star", OSCURIDAD, 8, 8, GROUND));

    std::cout << "Tablero del Juego Platas VS Zombies inicializado correctamente" << std::endl;
}
void Juego::cambiarTurno() {
    turnoActual = (turnoActual == LUZ) ? OSCURIDAD : LUZ;
    std::cout << "Cambio de turno. Ahora le toca a: "
        << (turnoActual == LUZ ? "Luz" : "Oscuridad") << std::endl;
}

void Juego::moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino) {
    //Que pieza se mueve
    Pieza* p = tablero->getPieza(fOrigen, cOrigen);

    if (p == nullptr) {
        std::cout << "Error: Casilla de origen vacía." << std::endl;
        return;
    }

    //A donde se mueve
    Pieza* ocupante = tablero->getPieza(fDestino, cDestino);

    if (ocupante == nullptr) {
        //Casilla libre, se mueve directamente
        tablero->moverPieza(fOrigen, cOrigen, fDestino, cDestino);
        std::cout << "Movimiento realizado." << std::endl;
    }
    else {
        //Casilla ocupada
        if (p->getBando() != ocupante->getBando()) {
            //Son de bandos opuestos->se pelean
            iniciarCombate(p, ocupante);
        }
        else {
            std::cout << "No puedes atacar a tus aliados." << std::endl;
            return;
        }
    }
    cambiarTurno();
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

void Juego:: iniciarCombate(Pieza* atacante, Pieza* defensor) {
    // 1. Llamamos a la Arena. Mario te devuelve el puntero de la pieza que gana.
    Pieza* ganador = arena->iniciarCombate(atacante, defensor);

    // 2. Si el ganador es el atacante, el defensor desaparece
    if (ganador == atacante) {
        std::cout << "¡" << atacante->getNombre() << " ha ganado el duelo!" << std::endl;

        // Importante: primero la quitamos del tablero de Hugo para que la casilla quede libre (nullptr)
        tablero->colocarPieza(defensor->filaInicial, defensor->colInicial, nullptr);

        delete defensor; // Liberamos la RAM
    }
    // 3. Si el ganador es el defensor, el atacante muere en el intento
    else if (ganador == defensor) {
        std::cout << "El defensor (" << defensor->getNombre() << ") se ha mantenido firme." << std::endl;

        tablero->colocarPieza(atacante->filaInicial, atacante->colInicial, nullptr);

        delete atacante;
    }
}

void Juego::dibujar() {
    std::cout << "Dibujando juego..." << std::endl;  
}

bool Juego::verificarVictoria() {
    return false;
}

Juego::~Juego() {
    delete tablero;
    delete arena;
}