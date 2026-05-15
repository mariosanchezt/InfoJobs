#pragma once
#include <SFML/Graphics.hpp>
#include "Tablero.h"
#include "Pieza.h"

#include <string>
#include <vector>
#include <map>

class Arena;
struct CombatienteArena;

enum EstadoPantalla {
    TABLERO,
    ARENA
};

class Renderer {

private:

    sf::RenderWindow& ventana;

    sf::Font fuente;
    bool fuenteCargada;

    static constexpr float TAM_CASILLA = 80.f;

    // Movemos tablero para dejar espacio a cementerios
    static constexpr float OFFSET_X = 140.f;
    static constexpr float OFFSET_Y = 20.f;

    EstadoPantalla estado;

    int filaSeleccionada;
    int colSeleccionada;

    int hechizoSeleccionado;

    // Cursor
    int cursorFila;
    int cursorCol;

    void dibujarCursor(Tablero* tablero);

    std::vector<std::pair<int, int>> movimientosDisponibles;

    // Sprites
    std::map<std::string, sf::Texture> texturas;
    bool texturasCargadas;

    // Colores
    sf::Color colorBlanco;
    sf::Color colorNegro;
    sf::Color colorGris;
    sf::Color colorSeleccion;
    sf::Color colorMovimiento;

    sf::Clock relojAnimacion;

    // TABLERO
    void dibujarTablero(Tablero* tablero);

    void dibujarPieza(Pieza* p, int fila, int col);

    void dibujarCasillaResaltada(int fila, int col);

    void dibujarMovimientosDisponibles();

    void dibujarBarraVida(
        float vida,
        float vidaMax,
        float x,
        float y,
        float ancho
    );

    void dibujarIndicadorTurno(
        Bando turno,
        Pieza* piezaSeleccionada
    );

    void dibujarPuntosDePoder(Tablero* tablero);

    void dibujarCementerio(
        const std::map<std::string, int>& cementerio,
        float x,
        float y,
        sf::Color colorTitulo,
        const std::string& titulo
    );

    std::string tipoMovimientoTexto(TipoMovimiento mov) const;

    std::string numeroTexto(float valor) const;

    std::string construirTextoPieza(Pieza* pieza) const;

    // ARENA
    void dibujarHUDArena(Pieza* p1, Pieza* p2);

    void dibujarCombatienteArena(
        const CombatienteArena& c,
        bool esLuz
    );

    std::string nombreArchivoSprite(
        const std::string& nombrePieza
    ) const;

public:

    Renderer(sf::RenderWindow& vent);

    bool cargarFuente(const std::string& ruta);

    void cargarSprites(const std::string& carpeta);

    void dibujarEstadoTablero(
        Tablero* tablero,
        Bando turno,
        bool* hechizosUsadosLuz,
        bool* hechizosUsadosOscuridad,
        int hechizoSeleccionado,
        const std::map<std::string, int>& cementerioLuz,
        const std::map<std::string, int>& cementerioOscuridad
    );

    void dibujarEstadoArena(const Arena& arena);

    void dibujarPantallaVictoria(Bando ganador);

    void dibujarPanelHechizos(
        Bando turno,
        bool* hechizosUsadosLuz,
        bool* hechizosUsadosOscuridad,
        int hechizoSeleccionado
    );

    void seleccionarCasilla(
        int fila,
        int col,
        Tablero* tablero
    );

    void deseleccionar();

    bool pixelACasilla(
        int px,
        int py,
        int& fila,
        int& col
    );

    EstadoPantalla getEstado() const {
        return estado;
    }

    void setEstado(EstadoPantalla e) {
        estado = e;
    }

    int getFilaSeleccionada() const {
        return filaSeleccionada;
    }

    int getColSeleccionada() const {
        return colSeleccionada;
    }

    // Cursor
    void moverCursor(int dFila, int dCol);

    int getCursorFila() const {
        return cursorFila;
    }

    int getCursorCol() const {
        return cursorCol;
    }

    // Animaciones
    bool ocultarMunecoCursor = false;

    void dibujarPiezaPixel(
        Pieza* p,
        float px,
        float py
    );

    sf::Vector2f getCentroCasilla(
        int fila,
        int col
    ) const;
};