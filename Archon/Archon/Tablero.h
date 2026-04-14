#pragma once
#include "Pieza.h"

class Tablero {
private:
	//creamos matriz 9x9 y usamos punteros para polimorfismo
	Pieza* casillas[9][9];

    int cicloOscilacion; //casillas oscilan y cambian de color

public:
    //constructor: prepara el tablero vacío
    Tablero();

    //coloca pieza en una coordenada específica
    void colocarPieza(int fila, int col, Pieza* p);

    //indica si una casilla es un punto de poder (hay 5)
    bool esPuntoDePoder(int f, int c);

    //detecta el final del juego
    bool hayGanador();
    //función para dibujar 
    void dibujar();
};