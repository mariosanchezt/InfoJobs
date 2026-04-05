#pragma once
#include <string>
//Pieza.h
// Los tres tipos de movimiento
enum TipoMovimiento { GROUND, FLYING, TELEPORT };

class Pieza {
public:
    // Atributos numéricos de combate y tablero
    float vida;
    float fuerza;
    float velAtaque;
    float intervaloAtaque; // Tiempo de recarga
    int radioMovimiento;

    // Casilla inicial (coordenadas en el tablero 9x9) 
    int filaInicial;
    int colInicial;

    TipoMovimiento mov; // Tierra, vuelo o teleport

    // Constructor: aquí "rellenas" la ficha con sus estadísticas
    Pieza(float v, float f, float va, float intA, int rm, int fIni, int cIni, TipoMovimiento t) {
        vida = v;
        fuerza = f;
        velAtaque = va;
        intervaloAtaque = intA;
        radioMovimiento = rm;
        filaInicial = fIni;
        colInicial = cIni;
        mov = t;
    }

    virtual ~Pieza() {} // Necesario para que C++ gestione bien la memoria

    virtual void atacar() = 0;
};