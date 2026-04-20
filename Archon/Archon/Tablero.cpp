#include "Tablero.h"
#include <iostream>

Tablero::Tablero() {
    ciclo = 1;             // Inicializamos el ciclo actual
    cicloOscilacion = 0;   // Contador de pasos para el ciclo

    // Inicializamos todo el tablero a nullptr (vacío)
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            casillas[i][j] = nullptr;
        }
    }
}

Pieza* Tablero::getPieza(int fila, int col) {
    if (fila >= 0 && fila < 9 && col >= 0 && col < 9) {
        return casillas[fila][col];
    }
    return nullptr;
}

void Tablero::colocarPieza(int fila, int col, Pieza* p) {
    if (fila >= 0 && fila < 9 && col >= 0 && col < 9) {
        casillas[fila][col] = p;
    }
}

void Tablero::moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino) {
    Pieza* p = getPieza(fOrigen, cOrigen);
    if (p != nullptr) {
        colocarPieza(fDestino, cDestino, p);
        casillas[fOrigen][cOrigen] = nullptr;
    }
}

bool Tablero::esPuntoDePoder(int f, int c) {
    // El tablero es de 9x9 y tiene 5 puntos de poder [cite: 136]
    // 1. Centro del tablero (4,4) [cite: 136]
    if (f == 4 && c == 4) return true;

    // 2. Centros de cada borde: (0,4), (8,4), (4,0), (4,8) [cite: 136]
    if ((f == 0 && c == 4) || (f == 8 && c == 4) ||
        (f == 4 && c == 0) || (f == 4 && c == 8)) {
        return true;
    }
    return false;
}

// Esta función la llamará el 'Juego' cada vez que termine un turno completo
void Tablero::avanzarCiclo() {
    // El ciclo va del 0 al 6 (representando: Blanco -> Gris -> Negro -> Gris -> Blanco...)
    ciclo++;
    if (ciclo > 6) ciclo = 0;
}

// Devuelve: 0 para Blanco (Luz), 1 para Negro (Oscuridad), 2 para Neutral (Gris)
int Tablero::getColorActual(int f, int c) {
    // 1. Identificamos si esta casilla es una de las que "oscila" (1/3 del tablero) 
    // Usamos una fórmula matemática para elegir 1 de cada 3 casillas de forma repartida
    if ((f * 9 + c) % 3 == 0) {
        // Estas casillas cambian según la variable 'ciclo'
        if (ciclo == 0 || ciclo == 6) return 0; // Blanco puro
        if (ciclo == 1 || ciclo == 5) return 2; // Gris (tirando a blanco)
        if (ciclo == 2 || ciclo == 4) return 2; // Gris (tirando a negro)
        if (ciclo == 3) return 1;               // Negro puro
    }

    // 2. Si no es oscilante, es una casilla fija tipo ajedrez [cite: 109, 136]
    return (f + c) % 2 == 0 ? 0 : 1;
}

void Tablero::dibujar() {
    // Implementación básica para ver algo por consola
    std::cout << "  0 1 2 3 4 5 6 7 8" << std::endl;
    for (int i = 0; i < 9; i++) {
        std::cout << i << " ";
        for (int j = 0; j < 9; j++) {
            if (casillas[i][j] == nullptr) {
                // Dibujamos un punto si está vacío
                std::cout << ". ";
            }
            else {
                // Llamamos al dibujar de la pieza (polimorfismo)
                casillas[i][j]->dibujar();
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}