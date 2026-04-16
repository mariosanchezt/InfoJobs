#pragma once
#include <vector>

class Tablero;
class Arena;
class Pieza;

class Juego {
private:
    Tablero* tablero;
    Arena* arena;
    Bando turnoActual;
    int hechizosRestantesLuz;
    int hechizosRestantesOscuridad;
    bool hechizosUsadosLuz[7];
    bool hechizosUsadosOscuridad[7];

public:
    Juego();
    ~Juego();

    void inicializarPartida();
    void cambiarTurno();
    void moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino);
    void lanzarHechizo(int idHechizo, Pieza* objetivo, int fDest = 0, int cDest = 0);
    void iniciarCombate(Pieza* p1, Pieza* p2);
    bool verificarVictoria();
    void dibujar();
};