#pragma once
#include <vector>
#include "Pieza.h"

class Tablero;
class Arena;
class Pieza;

class Juego {
private:
    Tablero* tablero;
    Arena* arena;
    Bando turnoActual;
    Bando ganadorPartida;
    int hechizosRestantesLuz;
    int hechizosRestantesOscuridad;
    bool hechizosUsadosLuz[7];
    bool hechizosUsadosOscuridad[7];
    Pieza* piezaRalentizada;
    float velAtaqueOriginalRalentizada;
    Pieza* piezaCongelada;
    float velAtaqueOriginalCongelada;

public:
    Juego();
    ~Juego();

    void inicializarPartida();
    void cambiarTurno();
    void moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino);
    void lanzarHechizo(int idHechizo, Pieza* objetivo, int fDest = 0, int cDest = 0);
    void iniciarCombate(Pieza* atacante, Pieza* defensor, int fAtac, int cAtac, int fDef, int cDef);
    bool verificarVictoria();
    void dibujar();

    Tablero* getTablero() const { return tablero; }
    Bando getTurnoActual() const { return turnoActual; }
    Bando getBandoGanador() const { return ganadorPartida; }
};