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
#include "IAJugador.h"
#include <iostream>

Juego::Juego() {
    tablero = new Tablero();
    arena = new Arena();

    turnoActual = LUZ;
    ganadorPartida = LUZ;
    // Al empezar la partida, todavía no hay victoria
    tipoVictoria = SIN_VICTORIA;

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
    piezaFortalecida = nullptr;
    fuerzaOriginalFortalecida = 0.f;

    piezaEscudo = nullptr;
    velAtaqueOriginalEscudo = 0.f;

    ia = nullptr;
    modoIA = false;

    iaSugerenciaLuz = new IAJugador(LUZ, MEDIO);
    iaSugerenciaOscuridad = new IAJugador(OSCURIDAD, MEDIO);

    turnosJugados = 0;

    // Temporizadores
    tiempoGeneral = TIEMPO_GENERAL_INICIAL;
    tiempoLuz = TIEMPO_BANDO_INICIAL;
    tiempoOscuridad = TIEMPO_BANDO_INICIAL;
}

void Juego::inicializarPartida() {
    // Reinicio de estado de partida
    turnoActual = LUZ;
    ganadorPartida = LUZ;
    tipoVictoria = SIN_VICTORIA;
    turnosJugados = 0;

    tiempoGeneral = TIEMPO_GENERAL_INICIAL;
    tiempoLuz = TIEMPO_BANDO_INICIAL;
    tiempoOscuridad = TIEMPO_BANDO_INICIAL;

    cementerioLuz.clear();
    cementerioOscuridad.clear();
    // MELEE
    for (int i = 0; i < 9; i++) {
        tablero->colocarPieza(i, 1, new PiezaMelee("Carnivora", LUZ, i, 1, GROUND));
        tablero->colocarPieza(i, 7, new PiezaMelee("Yeti", OSCURIDAD, i, 7, GROUND));
    }

    // PLANTAS
    tablero->colocarPieza(0, 0, new PiezaTanque("Cactus", LUZ, 0, 0, GROUND));
    tablero->colocarPieza(1, 0, new PiezaDistancia("Lanzaguisantes", LUZ, 1, 0, GROUND));
    tablero->colocarPieza(2, 0, new PiezaVoladora("Frutaestrella", LUZ, 2, 0, FLYING));
    tablero->colocarPieza(3, 0, new PiezaRapida("LanzaLechugas", LUZ, 3, 0, GROUND));
    tablero->colocarPieza(4, 0, new PiezaLider("Mazorca", LUZ, 4, 0, TELEPORT));
    tablero->colocarPieza(5, 0, new PiezaRapida("LanzaLechugas", LUZ, 5, 0, GROUND));
    tablero->colocarPieza(6, 0, new PiezaVoladora("Frutaestrella", LUZ, 6, 0, FLYING));
    tablero->colocarPieza(7, 0, new PiezaDistancia("Lanzaguisantes", LUZ, 7, 0, GROUND));
    tablero->colocarPieza(8, 0, new PiezaTanque("Cactus", LUZ, 8, 0, GROUND));

    // ZOMBIES
    tablero->colocarPieza(0, 8, new PiezaTanque("All-Star", OSCURIDAD, 0, 8, GROUND));
    tablero->colocarPieza(1, 8, new PiezaDistancia("Minero", OSCURIDAD, 1, 8, GROUND));
    tablero->colocarPieza(2, 8, new PiezaVoladora("Balloon", OSCURIDAD, 2, 8, FLYING));
    tablero->colocarPieza(3, 8, new PiezaRapida("Catapulta", OSCURIDAD, 3, 8, GROUND));
    tablero->colocarPieza(4, 8, new PiezaLider("Dr. Zomboss", OSCURIDAD, 4, 8, TELEPORT));
    tablero->colocarPieza(5, 8, new PiezaRapida("Catapulta", OSCURIDAD, 5, 8, GROUND));
    tablero->colocarPieza(6, 8, new PiezaVoladora("Balloon", OSCURIDAD, 6, 8, FLYING));
    tablero->colocarPieza(7, 8, new PiezaDistancia("Minero", OSCURIDAD, 7, 8, GROUND));
    tablero->colocarPieza(8, 8, new PiezaTanque("All-Star", OSCURIDAD, 8, 8, GROUND));

    std::cout << "Tablero inicializado correctamente" << std::endl;
}

void Juego::cambiarTurno() {
    turnoActual = (turnoActual == LUZ) ? OSCURIDAD : LUZ;
    turnosJugados++;

    std::cout << "Cambio de turno. Ahora le toca a: "
        << (turnoActual == LUZ ? "Luz" : "Oscuridad") << std::endl;
}

void Juego::moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino) {
    Pieza* p = tablero->getPieza(fOrigen, cOrigen);
    if (p == nullptr) {
        std::cout << "Error: Casilla de origen vacia." << std::endl;
        return;
    }

    if (!tablero->esMovimientoValido(fOrigen, cOrigen, fDestino, cDestino)) {
        return;
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
            return;
        }
    }

    cambiarTurno();
}

void Juego::lanzarHechizo(int idHechizo, Pieza* objetivo, int fDest, int cDest) {
    // Comprobar si el lider sigue vivo
    bool liderVivo = false;
    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            Pieza* p = tablero->getPieza(f, c);
            if (p != nullptr && p->getBando() == turnoActual && dynamic_cast<PiezaLider*>(p) != nullptr) {
                liderVivo = true;
            }
        }
    }
    if (!liderVivo) {
    std::cout << "No puedes lanzar hechizos sin lider." << std::endl;
        return;
    }
    
    bool* listaUsados = (turnoActual == LUZ) ? hechizosUsadosLuz : hechizosUsadosOscuridad;

    int indice = idHechizo - 1;

    if (indice < 0 || indice >= 7 || listaUsados[indice] == true) {
        std::cout << "Hechizo no valido o ya agotado." << std::endl;
        return;
    }

    switch (idHechizo) {
    case 1: // CURACION
        if (objetivo != nullptr && objetivo->getBando() == turnoActual) {
            objetivo->vida = objetivo->vidaMaxima;
            std::cout << "Curacion: " << objetivo->getNombre() << " restaurado." << std::endl;
        }
        break;
    case 2: // TELEPORT
        if (objetivo != nullptr && objetivo->getBando() == turnoActual) {
            tablero->colocarPieza(objetivo->filaInicial, objetivo->colInicial, nullptr);
            tablero->colocarPieza(fDest, cDest, objetivo);

            objetivo->filaInicial = fDest;
            objetivo->colInicial = cDest;

            std::cout << "Teletransporte: " << objetivo->getNombre() << " movido." << std::endl;
        }
        break;
    case 3: // DAÑO DIRECTO
        if (objetivo != nullptr && objetivo->getBando() != turnoActual) {
            objetivo->vida -= 50.f;
            if (objetivo->vida < 0.f) {
                objetivo->vida = 0.f;
            }

            std::cout << "Daño directo a " << objetivo->getNombre()
                << ". Vida restante: " << objetivo->vida << std::endl;
            if (objetivo->vida <= 0.f) {
                tablero->colocarPieza(objetivo->filaInicial, objetivo->colInicial, nullptr);
                registrarMuerte(objetivo);
                delete objetivo;
            }
        }
        break;
    case 4: // RALENTIZAR
        if (objetivo != nullptr && objetivo->getBando() != turnoActual) {
            if (piezaRalentizada != nullptr) {
                piezaRalentizada->velAtaque = velAtaqueOriginalRalentizada;
            }

            piezaRalentizada = objetivo;
            velAtaqueOriginalRalentizada = objetivo->velAtaque;
            objetivo->velAtaque *= 0.5f;

            std::cout << "Ralentizado: " << objetivo->getNombre() << std::endl;
        }
        break;
    case 5: // FORTALECER
        if (objetivo != nullptr && objetivo->getBando() == turnoActual) {
            if (piezaFortalecida != nullptr) {
                piezaFortalecida->fuerza = fuerzaOriginalFortalecida;
            }

            piezaFortalecida = objetivo;
            fuerzaOriginalFortalecida = objetivo->fuerza;
            objetivo->fuerza *= 1.5f;

            std::cout << "Fortalecido: " << objetivo->getNombre() << std::endl;
        }
        break;
    case 6: // ESCUDO
        if (objetivo != nullptr && objetivo->getBando() == turnoActual) {
            piezaEscudo = objetivo;
            velAtaqueOriginalEscudo = objetivo->velAtaque;

            std::cout << "Escudo aplicado a: " << objetivo->getNombre() << std::endl;
        }
        break;

    case 7: // CONGELAR
        if (objetivo != nullptr && objetivo->getBando() != turnoActual) {
            if (piezaCongelada != nullptr) {
                piezaCongelada->velAtaque = velAtaqueOriginalCongelada;
            }

            piezaCongelada = objetivo;
            velAtaqueOriginalCongelada = objetivo->velAtaque;
            objetivo->velAtaque = 0.f;

            std::cout << "Congelado: " << objetivo->getNombre() << std::endl;
        }
        break;
    }

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

        registrarMuerte(defensor);
        delete defensor;
        defensor = nullptr;

        //tablero->moverPieza(fAtac, cAtac, fDef, cDef);

        tablero->colocarPieza(fDef, cDef, atacante);
        tablero->colocarPieza(fAtac, cAtac, nullptr);
        atacante->filaInicial = fDef;
        atacante->colInicial = cDef;
    }
    else if (ganador == defensor) {
        std::cout << "El defensor (" << defensor->getNombre() << ") se ha mantenido firme." << std::endl;

        tablero->colocarPieza(fAtac, cAtac, nullptr);

        registrarMuerte(atacante);
        delete atacante;
    }
    else {
        std::cout << "¡Ambas piezas han muerto en combate!" << std::endl;

        tablero->colocarPieza(fAtac, cAtac, nullptr);
        tablero->colocarPieza(fDef, cDef, nullptr);

        registrarMuerte(atacante);
        registrarMuerte(defensor);

        delete atacante;
        delete defensor;
    }
}

void Juego::registrarMuerte(Pieza* pieza) {
    if (pieza == nullptr) return;
    if (pieza == piezaRalentizada) {
        piezaRalentizada = nullptr;
        velAtaqueOriginalRalentizada = 0.f;
    }

    if (pieza == piezaCongelada) {
        piezaCongelada = nullptr;
        velAtaqueOriginalCongelada = 0.f;
    }

    if (pieza == piezaFortalecida) {
        piezaFortalecida = nullptr;
        fuerzaOriginalFortalecida = 0.f;
    }

    if (pieza == piezaEscudo) {
        piezaEscudo = nullptr;
        velAtaqueOriginalEscudo = 0.f;
    }

    if (pieza->getBando() == LUZ) {
        cementerioLuz[pieza->getNombre()]++;
    }
    else {
        cementerioOscuridad[pieza->getNombre()]++;
    }
}

void Juego::dibujar() {
    std::cout << "Dibujando juego..." << std::endl;
}

bool Juego::verificarVictoria() {
    // Si ya se ha ganado por tiempo o puntuacion, no lo machacamos
    if (tipoVictoria == VICTORIA_TIEMPO || tipoVictoria == VICTORIA_PUNTUACION) {
        return true;
    }

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
                if (tablero->esPuntoDePoder(f, c)) {
                    puntosPoderLuz++;
                }
            }
            else {
                piezasOscuridad++;
                if (tablero->esPuntoDePoder(f, c)) {
                    puntosPoderOscuridad++;
                }
            }
        }
    }

    // Victoria por eliminacion total
    if (piezasOscuridad == 0) {
        ganadorPartida = LUZ;
        tipoVictoria = VICTORIA_ELIMINACION;
        return true;
    }

    if (piezasLuz == 0) {
        ganadorPartida = OSCURIDAD;
        tipoVictoria = VICTORIA_ELIMINACION;
        return true;
    }

    // Victoria por controlar los 5 puntos de poder
    if (puntosPoderLuz == 5) {
        ganadorPartida = LUZ;
        tipoVictoria = VICTORIA_PUNTOS_PODER;
        return true;
    }

    if (puntosPoderOscuridad == 5) {
        ganadorPartida = OSCURIDAD;
        tipoVictoria = VICTORIA_PUNTOS_PODER;
        return true;
    }

    tipoVictoria = SIN_VICTORIA;
    return false;
}

int Juego::getBajasLuz() const {
    int total = 0;
    for (const auto& par : cementerioLuz) {
        total += par.second;
    }

    return total;
}

int Juego::getBajasOscuridad() const {
    int total = 0;
    for (const auto& par : cementerioOscuridad) {
        total += par.second;
    }

    return total;
}

int Juego::getBajasTotales() const {
    return getBajasLuz() + getBajasOscuridad();
}

void Juego::actualizarTemporizadores(float dt) {
    if (dt <= 0.f) {
        return;
    }

    if (tipoVictoria != SIN_VICTORIA) {
        return;
    }

    tiempoGeneral -= dt;

    if (turnoActual == LUZ) {
        tiempoLuz -= dt;
    }
    else {
        tiempoOscuridad -= dt;
    }

    if (tiempoGeneral < 0.f) {
        tiempoGeneral = 0.f;
    }

    if (tiempoLuz < 0.f) {
        tiempoLuz = 0.f;
    }

    if (tiempoOscuridad < 0.f) {
        tiempoOscuridad = 0.f;
    }
}

bool Juego::verificarVictoriaPorTiempo() {
    if (tipoVictoria != SIN_VICTORIA) {
        return true;
    }

    if (tiempoLuz <= 0.f) {
        ganadorPartida = OSCURIDAD;
        tipoVictoria = VICTORIA_TIEMPO;
        return true;
    }

    if (tiempoOscuridad <= 0.f) {
        ganadorPartida = LUZ;
        tipoVictoria = VICTORIA_TIEMPO;
        return true;
    }

    if (tiempoGeneral <= 0.f) {
        resolverVictoriaPorPuntuacion();
        tipoVictoria = VICTORIA_PUNTUACION;
        return true;
    }

    return false;
}

int Juego::contarPiezasVivas(Bando bando) const {
    int total = 0;

    if (tablero == nullptr) {
        return total;
    }

    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            Pieza* p = tablero->getPieza(f, c);

            if (p != nullptr && p->getBando() == bando) {
                total++;
            }
        }
    }

    return total;
}

int Juego::contarPuntosPoder(Bando bando) const {
    int total = 0;

    if (tablero == nullptr) {
        return total;
    }

    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            if (!tablero->esPuntoDePoder(f, c)) {
                continue;
            }

            Pieza* p = tablero->getPieza(f, c);

            if (p != nullptr && p->getBando() == bando) {
                total++;
            }
        }
    }

    return total;
}

int Juego::calcularPuntuacionBando(Bando bando) const {
    int piezasVivas = contarPiezasVivas(bando);
    int puntosPoder = contarPuntosPoder(bando);

    int bajasPropias = (bando == LUZ)
        ? getBajasLuz()
        : getBajasOscuridad();

    int bajasEnemigas = (bando == LUZ)
        ? getBajasOscuridad()
        : getBajasLuz();

    int puntuacion =
        puntosPoder * 100 +
        bajasEnemigas * 200 +
        piezasVivas * 10 -
        bajasPropias * 75;

    if (puntuacion < 0) {
        puntuacion = 0;
    }

    return puntuacion;
}

void Juego::resolverVictoriaPorPuntuacion() {
    int puntuacionLuz = calcularPuntuacionBando(LUZ);
    int puntuacionOscuridad = calcularPuntuacionBando(OSCURIDAD);

    if (puntuacionLuz > puntuacionOscuridad) {
        ganadorPartida = LUZ;
        return;
    }

    if (puntuacionOscuridad > puntuacionLuz) {
        ganadorPartida = OSCURIDAD;
        return;
    }

    int puntosLuz = contarPuntosPoder(LUZ);
    int puntosOscuridad = contarPuntosPoder(OSCURIDAD);

    if (puntosLuz > puntosOscuridad) {
        ganadorPartida = LUZ;
        return;
    }

    if (puntosOscuridad > puntosLuz) {
        ganadorPartida = OSCURIDAD;
        return;
    }

    int bajasLuz = getBajasLuz();
    int bajasOscuridad = getBajasOscuridad();

    if (bajasLuz < bajasOscuridad) {
        ganadorPartida = LUZ;
        return;
    }

    if (bajasOscuridad < bajasLuz) {
        ganadorPartida = OSCURIDAD;
        return;
    }

    // Si sigue todo empatado, gana quien conserve mas tiempo individual.
    if (tiempoLuz >= tiempoOscuridad) {
        ganadorPartida = LUZ;
    }
    else {
        ganadorPartida = OSCURIDAD;
    }
}

void Juego::activarIA(Dificultad d) {
    delete ia;
    ia = new IAJugador(OSCURIDAD, d);
    modoIA = true;
}

MovimientoIA Juego::obtenerMovimientoIA() {
    if (ia == nullptr) {
        return { -1, -1, -1, -1, 0.f };
    }

    return ia->decidirMovimiento(tablero);
}

bool Juego::iaLanzarHechizo() {
    if (ia == nullptr || ia->getDificultad() != DIFICIL) return false;
    
    ia->setHechizosUsados(hechizosUsadosOscuridad);

    if (!ia->deberiaLanzarHechizo(tablero)) return false;

    int fObj = -1, cObj = -1;
    int hechizo = ia->elegirHechizo(tablero, fObj, cObj);

    if (hechizo == -1)return false;

    Pieza* objetivo = tablero->getPieza(fObj, cObj);
    lanzarHechizo(hechizo, objetivo, fObj, cObj);
    return true;
}

MovimientoIA Juego::obtenerSugerencia() {
    if (turnoActual == LUZ)
        return iaSugerenciaLuz->decidirMovimiento(tablero);
    else
        return iaSugerenciaOscuridad->decidirMovimiento(tablero);
}

bool Juego::liderVivo() const {
    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            Pieza* p = tablero->getPieza(f, c);
            if (p != nullptr && p->getBando() == turnoActual && dynamic_cast<PiezaLider*>(p) != nullptr)
                return true;
        }
    }
    return false;
}

Juego::~Juego() {
    delete tablero;
    delete arena;
    delete ia;
    delete iaSugerenciaLuz;
    delete iaSugerenciaOscuridad;
}