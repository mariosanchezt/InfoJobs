#pragma once
#include <SFML/Graphics.hpp>
#include "Tablero.h"
#include "Pieza.h"
#include "Juego.h"

#include <string>
#include <vector>
#include <map>

class Arena;
struct CombatienteArena;

enum EstadoPantalla { TABLERO, ARENA };
enum ModoHechizo { SIN_HECHIZO, HECHIZO_ALIADO, HECHIZO_ENEMIGO, HECHIZO_CASILLA };

// Tipos de animacion de hechizo
enum TipoAnimHechizo {
    ANIM_NINGUNA,
    ANIM_CURACION,
    ANIM_DANO,
    ANIM_RALENTIZAR,
    ANIM_FORTALECER,
    ANIM_ESCUDO,
    ANIM_CONGELAR
};

// Una animacion puntual sobre una casilla
struct AnimHechizo {
    int fila;
    int col;
    TipoAnimHechizo tipo;
    float tiempoRestante;
    float tiempoTotal;
};

// Estado de la animacion de teleport
enum FaseTeleport {
    FASE_NINGUNA,
    FASE_HUNDIR,
    FASE_MOVER,
    FASE_EMERGER
};

struct AnimTeleport {
    FaseTeleport fase = FASE_NINGUNA;
    Pieza* pieza = nullptr;

    int filaOrigen;
    int colOrigen;
    int filaDest;
    int colDest;

    float progreso = 0.f;
    float duracionFase = 0.3f;
};

class Renderer {
private:
    sf::RenderWindow& ventana;

    sf::Font fuente;
    bool fuenteCargada;

    // Tamaño general pensado para ventana 1200x900
    static constexpr float VENTANA_ANCHO = 1200.f;
    static constexpr float VENTANA_ALTO = 900.f;

    // Tablero
    static constexpr float TAM_CASILLA = 80.f;

    // Tablero centrado dejando espacio a cementerios laterales
    static constexpr float OFFSET_X = 240.f;
    static constexpr float OFFSET_Y = 20.f;

    // Cementerios laterales
    static constexpr float CEMENTERIO_ANCHO = 210.f;
    static constexpr float CEMENTERIO_ALTO = 500.f;
    static constexpr float CEMENTERIO_X_LUZ = 15.f;
    static constexpr float CEMENTERIO_X_OSCURIDAD = 975.f;
    static constexpr float CEMENTERIO_Y = 20.f;

    // Paneles de puntos de poder debajo de cada cementerio
    static constexpr float PODER_PANEL_ALTO = 125.f;
    static constexpr float PODER_PANEL_Y = CEMENTERIO_Y + CEMENTERIO_ALTO + 18.f;

    // HUD inferior principal del tablero
    static constexpr float HUD_Y = OFFSET_Y + 9.f * TAM_CASILLA + 5.f;
    static constexpr float HUD_ANCHO = 720.f;
    static constexpr float HUD_ALTO = 50.f;

    EstadoPantalla estado;

    int filaSeleccionada;
    int colSeleccionada;

    int hechizoSeleccionado;
    bool panelHechizosVisible;

    ModoHechizo modoHechizo;

    std::string nombrePiezaTeleport;

    int cursorFila;
    int cursorCol;

    void dibujarCursor(Tablero* tablero);

    std::vector<std::pair<int, int>> movimientosDisponibles;

    std::map<std::string, sf::Texture> texturas;
    bool texturasCargadas;

    sf::Color colorBlanco;
    sf::Color colorNegro;
    sf::Color colorGris;
    sf::Color colorSeleccion;
    sf::Color colorMovimiento;

    sf::Clock relojAnimacion;

    // Piezas con efectos activos para dibujar bordes persistentes
    Pieza* piezaCongelada = nullptr;
    Pieza* piezaRalentizada = nullptr;
    Pieza* piezaFortalecida = nullptr;
    Pieza* piezaEscudo = nullptr;

    // Animaciones puntuales de hechizo
    std::vector<AnimHechizo> animaciones;

    // Animacion de teleport
    AnimTeleport animTeleport;

    // Metodos internos tablero
    void dibujarTablero(Tablero* tablero);
    void dibujarPieza(Pieza* p, int fila, int col);

    void dibujarBordeEfecto(int fila, int col, sf::Color color, float t);
    void dibujarAnimaciones();
    void dibujarTeleportEnCurso();

    void dibujarCasillaResaltada(int fila, int col);
    void dibujarMovimientosDisponibles();

    void dibujarBarraVida(float vida, float vidaMax, float x, float y, float ancho);

    void dibujarIndicadorTurno(
        Bando turno,
        Pieza* piezaSeleccionada,
        Tablero* tablero,
        int hechizoSeleccionado
    );

    std::string tipoMovimientoTexto(TipoMovimiento mov) const;
    std::string numeroTexto(float valor) const;
    std::string construirTextoPieza(Pieza* pieza) const;

    void dibujarPuntosDePoder(Tablero* tablero);

    // Conteo de puntos de poder controlados por cada bando
    int contarPuntosPoder(Tablero* tablero, Bando bando) const;

    // Panel de puntos de poder lateral, debajo de cada cementerio
    void dibujarPanelPoderBando(
        Tablero* tablero,
        Bando bando,
        float x,
        float y,
        sf::Color colorTitulo,
        const std::string& titulo
    );

    void dibujarCirculosPoder(
        int puntosControlados,
        float x,
        float y,
        sf::Color colorActivo
    );

    // Cementerio visual con sprites grandes + contador x1, x2...
    void dibujarCementerio(
        const std::map<std::string, int>& cementerio,
        float x,
        float y,
        sf::Color colorTitulo,
        const std::string& titulo
    );

    void dibujarSpriteCementerio(
        const std::string& nombrePieza,
        float x,
        float y,
        float tam
    );

    // Metodos internos arena
    void dibujarHUDArena(Pieza* p1, Pieza* p2);
    void dibujarCombatienteArena(const CombatienteArena& c, bool esLuz);

    std::string nombreArchivoSprite(const std::string& nombrePieza) const;

public:
    Renderer(sf::RenderWindow& vent);

    bool cargarFuente(const std::string& ruta);
    void cargarSprites(const std::string& carpeta);

    void actualizarAnimaciones(float dt);

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

    // Pantalla final mejorada:
    // ganador -> plantas o zombies
    // tipoVictoria -> eliminacion total o puntos de poder
    // bajas -> resumen de piezas eliminadas
    void dibujarPantallaVictoria(
        Bando ganador,
        TipoVictoria tipoVictoria,
        int bajasLuz,
        int bajasOscuridad,
        int bajasTotales
    );

    void dibujarPanelHechizos(
        Bando turno,
        bool* hechizosUsadosLuz,
        bool* hechizosUsadosOscuridad,
        int hechizoSeleccionado
    );

    void seleccionarCasilla(int fila, int col, Tablero* tablero);
    void deseleccionar();

    bool pixelACasilla(int px, int py, int& fila, int& col);

    EstadoPantalla getEstado() const { return estado; }
    void setEstado(EstadoPantalla e) { estado = e; }

    int getFilaSeleccionada() const { return filaSeleccionada; }
    int getColSeleccionada() const { return colSeleccionada; }

    void moverCursor(int dFila, int dCol);

    int getCursorFila() const { return cursorFila; }
    int getCursorCol() const { return cursorCol; }

    void setModoHechizo(ModoHechizo modo) { modoHechizo = modo; }
    ModoHechizo getModoHechizo() const { return modoHechizo; }

    void setNombrePiezaTeleport(const std::string& nombre) {
        nombrePiezaTeleport = nombre;
    }

    void resetNombrePiezaTeleport() {
        nombrePiezaTeleport = "";
    }

    void setPanelHechizosVisible(bool v) {
        panelHechizosVisible = v;
    }

    bool getPanelHechizosVisible() const {
        return panelHechizosVisible;
    }

    // Piezas con efectos activos para pintar bordes permanentes
    void setPiezasConEfecto(
        Pieza* congelada,
        Pieza* ralentizada,
        Pieza* fortalecida,
        Pieza* escudo
    ) {
        piezaCongelada = congelada;
        piezaRalentizada = ralentizada;
        piezaFortalecida = fortalecida;
        piezaEscudo = escudo;
    }

    // Animacion puntual sobre una casilla
    void lanzarAnimHechizo(
        int fila,
        int col,
        TipoAnimHechizo tipo,
        float duracion = 0.5f
    );

    // Animacion de teleport
    void iniciarAnimTeleport(
        Pieza* pieza,
        int fOri,
        int cOri,
        int fDest,
        int cDest
    );

    bool teleportEnCurso() const {
        return animTeleport.fase != FASE_NINGUNA;
    }

    bool teleportTerminado() const {
        return animTeleport.fase == FASE_NINGUNA && animTeleport.pieza != nullptr;
    }

    void resetAnimTeleport() {
        animTeleport.pieza = nullptr;
        animTeleport.fase = FASE_NINGUNA;
    }

    bool ocultarMunecoCursor = false;

    void dibujarPiezaPixel(Pieza* p, float px, float py);
    void dibujarPiezaPixelEscala(Pieza* p, float px, float py, float escala);

    sf::Vector2f getCentroCasilla(int fila, int col) const;
};