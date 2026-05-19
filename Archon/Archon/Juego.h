#pragma once
#include <vector>
#include <map>
#include <string>

#include "Pieza.h"
#include "IAJugador.h"

class Tablero;
class Arena;
class Pieza;

class Juego {
private:
    Tablero* tablero;
    Arena* arena;

    Bando turnoActual;
    Bando ganadorPartida;

    int hechizosRestantesLuz;
    int hechizosRestantesOscuridad;
    bool hechizosUsadosLuz[7];
    bool hechizosUsadosOscuridad[7];

    // Efecto de ralentizar
    Pieza* piezaRalentizada;
    float velAtaqueOriginalRalentizada;

    // Efecto de congelar
    Pieza* piezaCongelada;
    float velAtaqueOriginalCongelada;

    // Efecto de fortalecer
    Pieza* piezaFortalecida;
    float fuerzaOriginalFortalecida;

    // Efecto de escudo
    Pieza* piezaEscudo;
    float velAtaqueOriginalEscudo;

    IAJugador* ia;
    bool modoIA;

    // Cementerio de piezas eliminadas
    std::map<std::string, int> cementerioLuz;
    std::map<std::string, int> cementerioOscuridad;

public:
    Juego();
    ~Juego();

    void inicializarPartida();
    void cambiarTurno();

    void moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino);
    void lanzarHechizo(int idHechizo, Pieza* objetivo, int fDest = 0, int cDest = 0);
    void iniciarCombate(Pieza* atacante, Pieza* defensor, int fAtac, int cAtac, int fDef, int cDef);

    bool verificarVictoria();
    void dibujar();

    void activarIA(Dificultad d);
    bool esModoIA() const { return modoIA; }
    MovimientoIA obtenerMovimientoIA();

    void registrarMuerte(Pieza* pieza);

    Tablero* getTablero() const { return tablero; }
    Bando getTurnoActual() const { return turnoActual; }
    Bando getBandoGanador() const { return ganadorPartida; }

    Pieza* getPiezaCongelada() const { return piezaCongelada; }
    Pieza* getPiezaRalentizada() const { return piezaRalentizada; }
    Pieza* getPiezaFortalecida() const { return piezaFortalecida; }
    Pieza* getPiezaEscudo() const { return piezaEscudo; }

    bool* getHechizosUsadosLuz() { return hechizosUsadosLuz; }
    bool* getHechizosUsadosOscuridad() { return hechizosUsadosOscuridad; }

    const std::map<std::string, int>& getCementerioLuz() const { return cementerioLuz; }
    const std::map<std::string, int>& getCementerioOscuridad() const { return cementerioOscuridad; }
};