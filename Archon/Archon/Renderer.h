#pragma once
#include <SFML/Graphics.hpp>
#include "Tablero.h"
#include "Pieza.h"
#include <string>
#include <vector>

// Estados posibles de la pantalla
enum EstadoPantalla { TABLERO, ARENA };

class Renderer {
private:
    sf::RenderWindow& ventana;

    sf::Font fuente;
    bool fuenteCargada;

    // Tamanho de cada casilla en pixeles
    static constexpr float TAM_CASILLA = 80.f;
    static constexpr float OFFSET_X = 20.f; // margen izquierdo
    static constexpr float OFFSET_Y = 20.f; // margen superior

    EstadoPantalla estado;

    // Casilla seleccionada (-1 si no hay ninguna)
    int filaSeleccionada;
    int colSeleccionada;

    // Casillas resaltadas como movimientos posibles
    std::vector<std::pair<int, int>> movimientosDisponibles;

    // Colores del tablero
    sf::Color colorBlanco;
    sf::Color colorNegro;
    sf::Color colorGris;
    sf::Color colorSeleccion;
    sf::Color colorMovimiento; // verde pa los movimientos disponibles

    // Metodos internos
    void dibujarTablero(Tablero* tablero);
    void dibujarPieza(Pieza* p, int fila, int col);
    void dibujarCasillaResaltada(int fila, int col);
    void dibujarMovimientosDisponibles();
    void dibujarBarraVida(float vida, float vidaMax, float x, float y, float ancho);
    void dibujarIndicadorTurno(Bando turno);

public:
    Renderer(sf::RenderWindow& vent);

    // Carga la fuente desde disco (llamar una vez al inicio)
    bool cargarFuente(const std::string& ruta);

    // Dibuja el tablero completo con todas las piezas
    void dibujarEstadoTablero(Tablero* tablero, Bando turno);

    // Dibuja la pantalla de arena con las dos piezas peleando
    void dibujarEstadoArena(Pieza* p1, Pieza* p2);

    // Selecciona una casilla y calcula sus movimientos disponibles
    void seleccionarCasilla(int fila, int col, Tablero* tablero);
    void deseleccionar();

    // Convierte coordenadas de pixel a casilla del tablero
    // Devuelve false si el click cayo fuera del tablero
    bool pixelACasilla(int px, int py, int& fila, int& col);

    EstadoPantalla getEstado() const { return estado; }
    void setEstado(EstadoPantalla e) { estado = e; }

    int getFilaSeleccionada() const { return filaSeleccionada; }
    int getColSeleccionada()  const { return colSeleccionada; }
};