#include "IAJugador.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include "PiezaLider.h"

IAJugador::IAJugador(Bando b, Dificultad d) {
	bando = b;
	dificultad = d;
}

MovimientoIA IAJugador::decidirMovimiento(Tablero* tablero) {
	switch (dificultad) {
	case FACIL:
		return elegirMovimientoAleatorio(tablero);
	case MEDIO:
	case DIFICIL:
		return elegirMejorMovimiento(tablero);
	default:
		return elegirMovimientoAleatorio(tablero);
	}
}

MovimientoIA IAJugador::elegirMovimientoAleatorio(Tablero* tablero) {
	std::vector<MovimientoIA> movimientos;


	for (int f = 0; f < 9; f++) {
		for (int c = 0; c < 9; c++) {
			Pieza* p = tablero->getPieza(f, c);
			if (p == nullptr || p->getBando() != bando) continue;

			for (int fD = 0; fD < 9; fD++) {
				for (int cD = 0; cD < 9; cD++) {
					if (f == fD && c == cD) continue;
					if (tablero->esMovimientoValido(f, c, fD, cD)) {
						Pieza* ocupante = tablero->getPieza(fD, cD);
						if (ocupante == nullptr || ocupante->getBando() != bando) {
							movimientos.push_back({ f, c, fD, cD, 0.f });
						}
					}
				}
			}
		}
	}
	if (movimientos.empty()) return{ -1, -1, -1, -1, 0.f };
	return movimientos[rand() % movimientos.size()];
}

MovimientoIA IAJugador::elegirMejorMovimiento(Tablero* tablero) {
	MovimientoIA mejor = { -1, -1, -1, -1, -999.f };

	for (int f = 0; f < 9; f++) {
		for (int c = 0; c < 9; c++) {
			Pieza* p = tablero->getPieza(f, c);
			if (p == nullptr || p->getBando() != bando) continue;

			for (int fD = 0; fD < 9; fD++) {
				for (int cD = 0; cD < 9; cD++) {
					if (f == fD && c == cD) continue;
					if (tablero->esMovimientoValido(f, c, fD, cD)) {
						Pieza* ocupante = tablero->getPieza(fD, cD);
						if (ocupante != nullptr && ocupante->getBando() == bando) continue;

						float punt = evaluarMovimiento(tablero, f, c, fD, cD);
						if (punt > mejor.puntuacion) {
							mejor = { f,c,fD,cD,punt };
						}
					}
				}
			}
		}
	}
	return mejor;
}

float IAJugador::evaluarMovimiento(Tablero* tablero, int fOri, int cOri, int fDest, int cDest) {
	float puntos = 0.f;

	//1. Si hay un enemigo en la casilla de destino se le ataca
	Pieza* objetivo = tablero->getPieza(fDest, cDest);
	if (objetivo != nullptr && objetivo->getBando() != bando) {
		puntos += 50.f;
		//Mas puntos si el enemigo tiene poca vida
		puntos += (1.f - objetivo->vida / objetivo->vidaMaxima) * 30.f;
	}

	//2. Moverse hacia el centro del tablero es bueno
	float distCentroAntes = abs(fOri - 4) + abs(cOri - 4);
	float distCentroDespues = abs(fDest - 4) + abs(cDest - 4);
	puntos += (distCentroAntes - distCentroDespues) * 2.f;

	//3. Moverse hacia el enemigo es bueno
	Bando enemigo = (bando == LUZ) ? OSCURIDAD : LUZ;
	int colEnemigo = (enemigo == LUZ) ? 0 : 8;
	float distEnemigAntes = abs(cOri - colEnemigo);
	float distEnemigoDespues = abs(cDest - colEnemigo);
	puntos += (distEnemigAntes - distEnemigoDespues) * 3.f;

	//4. Penalizar que el lider se arriesgue
	Pieza* pieza = tablero->getPieza(fOri, cOri);
	if (dynamic_cast<PiezaLider*>(pieza) != nullptr) {
		if (objetivo != nullptr) {
			puntos -= 40.f;
		}
	}

	return puntos;
}