#pragma once
#include "Pieza.h"
#include "Tablero.h"

enum Dificultad {FACIL, MEDIO, DIFICIL};

struct MovimientoIA {
	int fOrigen, cOrigen;
	int fDestino, cDestino;
	float puntuacion;
};

class IAJugador {
private:
	Bando bando;
	Dificultad dificultad;

	MovimientoIA elegirMovimientoAleatorio(Tablero* tablero);
	MovimientoIA elegirMejorMovimiento(Tablero* tablero);
	float evaluarMovimiento(Tablero* tablero, int fOri, int cOri, int fDest, int cDest);

public:
	IAJugador(Bando b, Dificultad d);
	MovimientoIA decidirMovimiento(Tablero* tablero);
};