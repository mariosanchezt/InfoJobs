#pragma once
#include <string>
// Los tres tipos de movimiento y los dos bandos
enum TipoMovimiento { GROUND, FLYING, TELEPORT };

//enum Bando {PLANTAS, ZOMBIES};
enum Bando { LUZ, OSCURIDAD };


class Pieza {
protected:
    Bando bando;
    std::string nombre;
public:
    // Atributos numéricos de combate y tablero
    float vida;
    float vidaMaxima;
    float fuerza;
    float velAtaque;
    float intervaloAtaque; // Tiempo de recarga
    int radioMovimiento;

    // Casilla inicial (coordenadas en el tablero 9x9) 
    int filaInicial;
    int colInicial;

    TipoMovimiento mov; // Tierra, vuelo o teleport

    // Constructor: aquí "rellenas" la ficha con sus estadísticas
    Pieza(std::string n, Bando b, float v, float f, float va, float intA, int rm, int fIni, int cIni, TipoMovimiento t) {
        nombre = n;
        bando = b;
        vida = v;
        vidaMaxima = v; //Al principio, la maxima es con la que nace
        fuerza = f;
        velAtaque = va;
        intervaloAtaque = intA;
        radioMovimiento = rm;
        filaInicial = fIni;
        colInicial = cIni;
        mov = t;
    }

    virtual ~Pieza() {} // Necesario para que C++ gestione bien la memoria
    //cuando usamos "new", esto es necesario

    Bando getBando() const { return bando; }
    std::string getNombre() const { return nombre; }

    virtual void atacar(Pieza* enemigo) = 0;
    virtual void dibujar() = 0;
};