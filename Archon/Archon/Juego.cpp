#include "Juego.h"
#include "Tablero.h"
#include "Arena.h"
#include "Pieza.h"
#include "PiezaMelee.h"
#include "PiezaDistancia.h"
#include "PiezaRapida.h"
#include "PiezaTanque.h"
#include "PiezaVoladora.h"
#include "PiezaLider.h"
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
    piezaRalentizada = nullptr;
    velAtaqueOriginalRalentizada = 0.f;
    piezaCongelada = nullptr;
    velAtaqueOriginalCongelada = 0.f;
}

void Juego::inicializarPartida() {
    // MELEE
    for (int i = 0; i < 9; i++) {
        tablero->colocarPieza(i, 1, new PiezaMelee("Carnivora", LUZ, i, 1, GROUND));
        tablero->colocarPieza(i, 7, new PiezaMelee("Supercerebroz", OSCURIDAD, i, 7, GROUND));
    }

    // BANDO LUZ - PLANTAS, columna exterior
    tablero->colocarPieza(0, 0, new PiezaTanque("Pomelo", LUZ, 0, 0, GROUND));
    tablero->colocarPieza(1, 0, new PiezaDistancia("Lanzaguisantes", LUZ, 1, 0, GROUND));
    tablero->colocarPieza(2, 0, new PiezaVoladora("Mazorca", LUZ, 2, 0, FLYING));
    tablero->colocarPieza(3, 0, new PiezaRapida("Girasol", LUZ, 3, 0, GROUND));
    tablero->colocarPieza(4, 0, new PiezaLider("Dave el Loco", LUZ, 4, 0, TELEPORT));

    tablero->colocarPieza(5, 0, new PiezaRapida("Girasol", LUZ, 5, 0, GROUND));
    tablero->colocarPieza(6, 0, new PiezaVoladora("Mazorca", LUZ, 6, 0, FLYING));
    tablero->colocarPieza(7, 0, new PiezaDistancia("Lanzaguisantes", LUZ, 7, 0, GROUND));
    tablero->colocarPieza(8, 0, new PiezaTanque("Pomelo", LUZ, 8, 0, GROUND));

    // BANDO OSCURIDAD - ZOMBIES, columna exterior
    tablero->colocarPieza(0, 8, new PiezaTanque("All-Star", OSCURIDAD, 0, 8, GROUND));
    tablero->colocarPieza(1, 8, new PiezaDistancia("Soldado", OSCURIDAD, 1, 8, GROUND));
    tablero->colocarPieza(2, 8, new PiezaVoladora("Ingeniero", OSCURIDAD, 2, 8, FLYING));
    tablero->colocarPieza(3, 8, new PiezaRapida("Zombidito", OSCURIDAD, 3, 8, GROUND));
    tablero->colocarPieza(4, 8, new PiezaLider("Dr. Zombi", OSCURIDAD, 4, 8, TELEPORT));

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
    Pieza* p = tablero->getPieza(fOrigen, cOrigen);

    if (p == nullptr) {
        std::cout << "Error: Casilla de origen vacia." << std::endl;
        return; // sin cambio de turno
    }

    if (!tablero->esMovimientoValido(fOrigen, cOrigen, fDestino, cDestino)) {
        return; // sin cambio de turno
    }

    Pieza* ocupante = tablero->getPieza(fDestino, cDestino);

    if (ocupante == nullptr) {
        tablero->moverPieza(fOrigen, cOrigen, fDestino, cDestino);
        std::cout << "Movimiento realizado." << std::endl;
    }
    else {
        if (p->getBando() != ocupante->getBando()) {
            iniciarCombate(p, ocupante, fOrigen, cOrigen, fDestino, cDestino);
        }
        else {
            std::cout << "No puedes atacar a tus aliados." << std::endl;
            return; // sin cambio de turno
        }
    }

    cambiarTurno();
}

void Juego::lanzarHechizo(int idHechizo, Pieza* objetivo, int fDest, int cDest) {
    bool* listaUsados = (turnoActual == LUZ) ? hechizosUsadosLuz : hechizosUsadosOscuridad;

    int indice = idHechizo - 1;

    if (indice < 0 || indice >= 7 || listaUsados[indice] == true) {
        std::cout << "Hechizo no valido o ya agotado." << std::endl;
        return;
    }

    switch (idHechizo) {
    case 1: // HEAL
        if (objetivo != nullptr && objetivo->getBando() == turnoActual) {
            objetivo->vida = objetivo->vidaMaxima;
            std::cout << "Curacion: " << objetivo->getNombre() << " restaurado" << std::endl;
        }
        break;
    case 2: // TELEPORT
        if (objetivo != nullptr && objetivo->getBando() == turnoActual) {
            tablero->colocarPieza(objetivo->filaInicial, objetivo->colInicial, nullptr);
            tablero->colocarPieza(fDest, cDest, objetivo);
            objetivo->filaInicial = fDest;
            objetivo->colInicial = cDest;
            std::cout << "Teletransporte: " << objetivo->getNombre() << "movido" << std::endl;
        }
        break;
    case 3: //DAÑO DIRECTO
        if (objetivo != nullptr && objetivo->getBando() != turnoActual) {
            objetivo->vida -= 50.f;
            if (objetivo->vida < 0.f) objetivo->vida = 0.f;
            std::cout << "Daño directo a " << objetivo->getNombre() << ". Vida restante: " << objetivo->vida << std::endl;
            if (objetivo->vida <= 0.f) {
                tablero->colocarPieza(objetivo->filaInicial, objetivo->colInicial, nullptr);
                delete objetivo;
            }
        }
        break;

    case 4: //RALENTIZAR
        if (objetivo != nullptr && objetivo->getBando() != turnoActual) {
            // Restaurar pieza anterior si habia una ralentizada
            if (piezaRalentizada != nullptr) piezaRalentizada->velAtaque = velAtaqueOriginalRalentizada;
            piezaRalentizada = objetivo;
            velAtaqueOriginalRalentizada = objetivo->velAtaque;
            objetivo->velAtaque *= 0.5f;
            std::cout << "Ralentizado: " << objetivo->getNombre() << std::endl;
        }
        break;
    case 5: // FORTALECER
        if (objetivo != nullptr && objetivo->getBando() == turnoActual) {
            objetivo->fuerza *= 1.5f;
            std::cout << "Fortalecido: " << objetivo->getNombre() << std::endl;
        }
        break;
    case 6: // ESCUDO
        if (objetivo != nullptr && objetivo->getBando() == turnoActual) {
            if (piezaCongelada != nullptr)
                piezaCongelada->velAtaque = velAtaqueOriginalCongelada;
            piezaCongelada = objetivo;
            velAtaqueOriginalCongelada = objetivo->velAtaque;
            objetivo->velAtaque = 0.f;
            std::cout << "Escudo: " << objetivo->getNombre() << " protegido." << std::endl;
        }
        break;
    case 7: // CONGELAR
        if (objetivo != nullptr && objetivo->getBando() != turnoActual) {
            if (piezaCongelada != nullptr)
                piezaCongelada->velAtaque = velAtaqueOriginalCongelada;
            piezaCongelada = objetivo;
            velAtaqueOriginalCongelada = objetivo->velAtaque;
            objetivo->velAtaque = 0.f;
            std::cout << "Congelado: " << objetivo->getNombre() << std::endl;
        }
        break;
    listaUsados[indice] = true;

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

void Juego::iniciarCombate(Pieza* atacante, Pieza* defensor, int fAtac, int cAtac, int fDef, int cDef) {
    Pieza* ganador = arena->iniciarCombateAutomatico(atacante, defensor);

    if (ganador == atacante) {
        std::cout << "¡" << atacante->getNombre() << " ha ganado el duelo!" << std::endl;
        tablero->colocarPieza(fDef, cDef, nullptr);
        delete defensor;
        tablero->moverPieza(fAtac, cAtac, fDef, cDef);
    }
    else if (ganador == defensor) {
        std::cout << "El defensor (" << defensor->getNombre() << ") se ha mantenido firme." << std::endl;
        tablero->colocarPieza(fAtac, cAtac, nullptr);
        delete atacante;
    }
    else {
        std::cout << "¡Ambas piezas han muerto en combate!" << std::endl;
        tablero->colocarPieza(fAtac, cAtac, nullptr);
        tablero->colocarPieza(fDef, cDef, nullptr);
        delete atacante;
        delete defensor;
    }
}

void Juego::dibujar() {
    std::cout << "Dibujando juego..." << std::endl;
}

bool Juego::verificarVictoria() {
    int piezasLuz = 0;
    int piezasOscuridad = 0;
    int puntosPoderLuz = 0;
    int puntosPoderOscuridad = 0;

    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            Pieza* p = tablero->getPieza(f, c);
            if (p == nullptr) continue;

            if (p->getBando() == LUZ) {
                piezasLuz++;
                if (tablero->esPuntoDePoder(f, c)) puntosPoderLuz++;
            }
            else {
                piezasOscuridad++;
                if (tablero->esPuntoDePoder(f, c)) puntosPoderOscuridad++;
            }
        }
    }

    // Condicion 1: un bando se queda sin piezas
    if (piezasOscuridad == 0) { ganadorPartida = LUZ;       return true; }
    if (piezasLuz == 0)       { ganadorPartida = OSCURIDAD; return true; }

    // Condicion 2: un bando controla los 5 puntos de poder
    if (puntosPoderLuz == 5)       { ganadorPartida = LUZ;       return true; }
    if (puntosPoderOscuridad == 5) { ganadorPartida = OSCURIDAD; return true; }

    return false;
}

Juego::~Juego() {
    delete tablero;
    delete arena;
}