#pragma once
#include <SFML/Graphics.hpp>
#include "Pieza.h"
#include <vector>

class AudioManager;

struct Proyectil {
    sf::Vector2f pos;
    sf::Vector2f vel;
    bool activo;
    bool esDeLuz;
};

struct Obstaculo {
    sf::Vector2f pos;
    float tam;
};

// Estado de animacion del combatiente en la arena
enum EstadoAnimArena { ANIM_IDLE, ANIM_ATTACK };

struct CombatienteArena {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float tiempoRecarga;
    bool teclaDsparoPulsada;
    Pieza* pieza;
    float multiplicadorVelocidad;

    // Campos de animacion por frames (inicializados en iniciarCombate)
    EstadoAnimArena estadoAnim = ANIM_IDLE;
    int   frameActual = 0;
    float timerFrame = 0.f;
    float duracionFrame = 0.1f;
    float tiempoAtaque = 0.f;
    float tiempoRecargaMax = 0.5f;
};
struct OndaMelee {
    sf::Vector2f pos;
    float radioMax;
    float tiempoVida;
    float duracionMax;
    bool esDeLuz;
};

class Arena {
private:
    // Arena grande y cuadrada para ventana 1200x900
    static constexpr float ANCHO = 860.f;
    static constexpr float ALTO = 860.f;
    static constexpr float OFFSET_X = 170.f;
    static constexpr float OFFSET_Y = 20.f;

    static constexpr float VEL_PROYECTIL = 280.f;
    static constexpr float TAM_PIEZA = 24.f;
    static constexpr float TAM_PROYECTIL = 12.f;

    CombatienteArena combatiente1; // LUZ / PLANTAS — izquierda — WASD + Espacio
    CombatienteArena combatiente2; // OSCURIDAD / ZOMBIES — derecha — Flechas + Enter

    std::vector<Proyectil>  proyectiles;
    std::vector<Obstaculo>  obstaculos;
    std::vector<OndaMelee>  ondasMelee;

    bool modoIA;
    bool   combateTerminado;
    Pieza* ganador;

    AudioManager* audio;

    void generarObstaculos();
    void moverCombatiente(CombatienteArena& c, sf::Vector2f dir, float dt);
    void crearProyectil(CombatienteArena& tirador, CombatienteArena& objetivo);
    void actualizarProyectiles(float dt);
    void comprobarColisiones();
    bool colisionaConObstaculo(sf::Vector2f pos, float radio);
    bool dentroDeArena(sf::Vector2f pos);

public:
    Arena();

    void iniciarCombate(Pieza* p1, Pieza* p2);
    void update(float dt);
    void setMultiplicadorVelocidad(Pieza* p, float n);

    // Activa la anim de ataque en un combatiente (se llama al disparar)
    void notificarDisparo(CombatienteArena& c);

    bool   haTerminado() const { return combateTerminado; }
    Pieza* getGanador()  const { return ganador; }
    void setModoIA(bool ia) { modoIA = ia; }

    void limpiar() {
        combatiente1.pieza = nullptr;
        combatiente2.pieza = nullptr;
        proyectiles.clear();
    }

    const CombatienteArena& getCombatiente1()        const { return combatiente1; }
    const CombatienteArena& getCombatiente2()        const { return combatiente2; }
    const std::vector<Proyectil>& getProyectiles()   const { return proyectiles; }
    const std::vector<Obstaculo>& getObstaculos()    const { return obstaculos; }
    const std::vector<OndaMelee>& getOndasMelee()    const { return ondasMelee; }

    static constexpr float getAncho() { return ANCHO; }
    static constexpr float getAlto() { return ALTO; }
    static constexpr float getOffsetX() { return OFFSET_X; }
    static constexpr float getOffsetY() { return OFFSET_Y; }
    static constexpr float getTamPieza() { return TAM_PIEZA; }
    static constexpr float getTamProyectil() { return TAM_PROYECTIL; }
    void setAudio(AudioManager* a) { audio = a; }

    Pieza* iniciarCombateAutomatico(Pieza* p1, Pieza* p2);
};