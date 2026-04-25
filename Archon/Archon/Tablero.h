#pragma once
#include "Pieza.h"

class Tablero {
private:
    Pieza* casillas[9][9];

    int cicloOscilacion;
    int ciclo;

public:
    Tablero();

    Pieza* getPieza(int fila, int col);

    void colocarPieza(int fila, int col, Pieza* p);

    bool esMovimientoValido(int fOrigen, int cOrigen, int fDestino, int cDestino);

    void moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino);

    bool esPuntoDePoder(int f, int c);

    void avanzarCiclo();
    int getColorActual(int f, int c);

    void dibujar();
};