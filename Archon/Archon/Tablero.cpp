#include "Tablero.h"
#include "PiezaMelee.h"
#include "PiezaDistancia.h"
#include "PiezaRapida.h"
#include "PiezaTanque.h"
#include "PiezaVoladora.h"
#include <iostream>

Tablero::Tablero() {
	cicloOscilacion = 0; //eestado inicial para empezar

	for (int i = 0; i < 9; i++) {
		for (int j = 0;j < 9;j++) {
			casillas[i][j] = nullptr;
		}
	}
}


//Getter - mira que pieza hay en la casila si esque hay alguna
Pieza* Tablero::getPieza(int fila, int col) {
	// 1.comprobamos que las coord estén dentro del tablero (0 a 8)
	if (fila >= 0 && fila < 9 && col >= 0 && col < 9) {
		return casillas[fila][col]; //devolvemos lo que haya (pieza o nullptr)
	}

	// 2.si las coordenadas están fuera, devolvemos nullptr
	return nullptr;
}

//Setter - pone nullptr en la casilla si la pieza se ha movido o pone pieza si ha llegado
void Tablero::colocarPieza(int fila, int col, Pieza* p) {
	if (fila >= 0 && fila < 9 && col >= 0 && col < 9) {
		casillas[fila][col] = p;
	}
}
void Tablero::moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino) {
	// 1.guardamos la pieza que se va a mover
	Pieza* p = getPieza(fOrigen, cOrigen);

	if (p != nullptr) {
		// 2.ponemos la pieza en la nueva casilla
		colocarPieza(fDestino, cDestino, p);

		// 3.vaciamos la casilla vieja poniendo nullptr
		// IMPORTANTE - pieza vieja no 2 sitios a la vez
		casillas[fOrigen][cOrigen] = nullptr;

		// 4. (Opcional) Actualizamos las coordenadas internas de la pieza
		// p->filaActual = fD; 
		// p->colActual = cD;
	}
}

bool Tablero::esPuntoDePoder(int f, int c) {
	//el centro exacto (4,4)
	if (f == 4 && c == 4) return true;

	//4 centros de los bordes: (0,4), (8,4), (4,0), (4,8)
	if ((f == 0 && c == 4) || (f == 8 && c == 4) ||
		(f == 4 && c == 0) || (f == 4 && c == 8)) {
		return true;
	}

	return false;
}

//void Tablero::inicializarPartida() {
//	//MELEE: carnivora / supercerebroz - 8x por bando en 2º columna
//	for (int i = 0; i < 9;i++) {
//		if (i = 4) continue; //hueco central fila libre
//		// Plantas (Columna 1)
//		colocarPieza(i, 1, new PiezaMelee(LUZ, i, 1));
//
//		// Zombies (Columna 7)
//		colocarPieza(i, 7, new PiezaMelee(OSCURIDAD, i, 7));
//	}
//	
//	// --- BANDO LUZ (Plantas - Columna 0) ---
//	colocarPieza(0, 0, new PiezaTanque(LUZ, 0, 0));    // Pomelo
//	colocarPieza(1, 0, new PiezaDistancia(LUZ, 1, 0)); // Lanzaguisantes
//	colocarPieza(2, 0, new PiezaVoladora(LUZ, 2, 0));  // Mazorca
//	colocarPieza(3, 0, new PiezaRapida(LUZ, 3, 0));    // Girasol
//	// El 4,0 suele ser el Líder (Dave el Loco / Wizard)
//
//	// --- BANDO OSCURIDAD (Zombies - Columna 8) ---
//	colocarPieza(0, 8, new PiezaTanque(OSCURIDAD, 0, 8));    // All-Star
//	colocarPieza(1, 8, new PiezaDistancia(OSCURIDAD, 1, 8)); // Soldado
//	colocarPieza(2, 8, new PiezaVoladora(OSCURIDAD, 2, 8));  // Ingeniero
//	colocarPieza(3, 8, new PiezaRapida(OSCURIDAD, 3, 8));    // Zombidito
//}


void Tablero::dibujar() {
	std::cout << "Dibujando tablero..." << std::endl;
}
