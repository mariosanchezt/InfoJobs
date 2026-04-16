#pragma once
#include "Pieza.h"

class Tablero {
private:
	//creamos matriz 9x9 y usamos punteros para polimorfismo
	Pieza* casillas[9][9];
    //getter-esto devuelve un puntero a la pieza que haya en esa casilla
    Pieza* getPieza(int fila, int col);

    int cicloOscilacion; //casillas oscilan y cambian de color

public:
    //constructor: prepara el tablero vacío
    Tablero();

    //coloca todos los personajes
    void inicializarPartida();

    //setter-coloca pieza en una coordenada específica
    void colocarPieza(int fila, int col, Pieza* p);

    void moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino);

    //indica si una casilla es un punto de poder (hay 5)
    bool esPuntoDePoder(int f, int c);

    //función para dibujar 
    void dibujar();
};