#include "Tablero.h"
#include <iostream>

Tablero::Tablero() {
	cicloOscilacion = 0; //eestado inicial para empezar

	for (int i = 0; i < 9; i++) {
		for (int j = 0;j < 9;j++) {
			casillas[i][j] = nullptr;
		}
	}
}

void Tablero::colocarPieza(int fila, int col, Pieza* p) {
	//se comprueba que coord esté edntro de tablero (de 0-8)
	if (fila >= 0 && fila < 9 && col >= 0 && col < 9) {
		casillas[fila][col] = p;
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
void Tablero::dibujar() {
	std::cout << "Dibujando tablero..." << std::endl;
}