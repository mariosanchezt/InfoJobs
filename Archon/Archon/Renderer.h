#pragma once
#include <SFML/Graphics.hpp>
#include "Tablero.h"
#include "Pieza.h"
#include <string>
#include <vector>
#include <map>

// Estados posibles de la pantalla
enum EstadoPantalla { TABLERO, ARENA };

class Renderer {
private:
    sf::RenderWindow& ventana;

    sf::Font fuente;
    bool fuenteCargada;

    // Tamanho de cada casilla en pixeles
    static constexpr float TAM_CASILLA = 80.f;
    static constexpr float OFFSET_X = 20.f;
    static constexpr float OFFSET_Y = 20.f;

    EstadoPantalla estado;

    // Casilla seleccionada (-1 si no hay ninguna)
    int filaSeleccionada;
    int colSeleccionada;

    // Casillas resaltadas como movimientos posibles
    std::vector<std::pair<int, int>> movimientosDisponibles;

    // Texturas de los sprites — mapa nombre_archivo -> textura
    std::map<std::string, sf::Texture> texturas;
    bool texturasCargadas;

    // Colores
    sf::Color colorBlanco;
    sf::Color colorNegro;
    sf::Color colorGris;
    sf::Color colorSeleccion;
    sf::Color colorMovimiento;

    // Metodos internos tablero
    void dibujarTablero(Tablero* tablero);
    void dibujarPieza(Pieza* p, int fila, int col);
    void dibujarCasillaResaltada(int fila, int col);
    void dibujarMovimientosDisponibles();
    void dibujarBarraVida(float vida, float vidaMax, float x, float y, float ancho);
    void dibujarIndicadorTurno(Bando turno);

    // Mapea nombre de pieza a nombre de archivo PNG
    std::string nombreArchivoSprite(const std::string& nombrePieza) const;

public:
    Renderer(sf::RenderWindow& vent);

    bool cargarFuente(const std::string& ruta);

    // Carga los sprites PNG desde la carpeta indicada
    void cargarSprites(const std::string& carpeta);

    // Dibuja el tablero completo con todas las piezas
    void dibujarEstadoTablero(Tablero* tablero, Bando turno);

    // Dibuja la pantalla de arena con las dos piezas peleando
    void dibujarEstadoArena(Pieza* p1, Pieza* p2);

    // Selecciona una casilla y calcula sus movimientos disponibles
    void seleccionarCasilla(int fila, int col, Tablero* tablero);
    void deseleccionar();

    // Convierte coordenadas de pixel a casilla del tablero
    bool pixelACasilla(int px, int py, int& fila, int& col);

    EstadoPantalla getEstado() const { return estado; }
    void setEstado(EstadoPantalla e) { estado = e; }

    int getFilaSeleccionada() const { return filaSeleccionada; }
    int getColSeleccionada()  const { return colSeleccionada; }
};