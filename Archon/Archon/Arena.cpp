#include "Arena.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

static constexpr float MARGEN_PATIO_IZQ = 170.f;
static constexpr float MARGEN_PATIO_DER = 30.f;
static constexpr float MARGEN_PATIO_SUP = 85.f;
static constexpr float MARGEN_PATIO_INF = 80.f;

Arena::Arena() {
    combateTerminado = false;
    ganador = nullptr;
}

void Arena::iniciarCombate(Pieza* p1, Pieza* p2) {
    combateTerminado = false;
    ganador = nullptr;
    proyectiles.clear();

    float patioMinX = OFFSET_X + MARGEN_PATIO_IZQ;
    float patioMaxX = OFFSET_X + ANCHO - MARGEN_PATIO_DER;
    float patioMinY = OFFSET_Y + MARGEN_PATIO_SUP;
    float patioMaxY = OFFSET_Y + ALTO - MARGEN_PATIO_INF;

    CombatienteArena* cLuz = (p1->getBando() == LUZ) ? &combatiente1 : &combatiente2;
    CombatienteArena* cOscuridad = (p1->getBando() == LUZ) ? &combatiente2 : &combatiente1;

    Pieza* piezaLuz = (p1->getBando() == LUZ) ? p1 : p2;
    Pieza* piezaOscuridad = (p1->getBando() == LUZ) ? p2 : p1;

    cLuz->pieza = piezaLuz;
    cLuz->pos = sf::Vector2f(patioMinX + 60.f, (patioMinY + patioMaxY) / 2.f);
    cLuz->vel = sf::Vector2f(0.f, 0.f);
    cLuz->tiempoRecarga = 0.f;
    cLuz->teclaDsparoPulsada = false;
    cLuz->multiplicadorVelocidad = 1.f;

    cOscuridad->pieza = piezaOscuridad;
    cOscuridad->pos = sf::Vector2f(patioMaxX - 60.f, (patioMinY + patioMaxY) / 2.f);
    cOscuridad->vel = sf::Vector2f(0.f, 0.f);
    cOscuridad->tiempoRecarga = 0.f;
    cOscuridad->teclaDsparoPulsada = false;
    cOscuridad->multiplicadorVelocidad = 1.f;

    generarObstaculos();

    std::cout << "=== COMBATE INTERACTIVO ===" << std::endl;
    std::cout << piezaLuz->getNombre() << " (Plantas/WASD+Space) VS "
        << piezaOscuridad->getNombre() << " (Zombies/Flechas+Enter)" << std::endl;
}

void Arena::generarObstaculos() {
    obstaculos.clear();

    float patioMinX = OFFSET_X + MARGEN_PATIO_IZQ;
    float patioMaxX = OFFSET_X + ANCHO - MARGEN_PATIO_DER;
    float patioMinY = OFFSET_Y + MARGEN_PATIO_SUP;
    float patioMaxY = OFFSET_Y + ALTO - MARGEN_PATIO_INF;

    int numObs = 4 + rand() % 4;
    int intentos = 0;

    while ((int)obstaculos.size() < numObs && intentos < 150) {
        intentos++;

        Obstaculo obs;

        obs.tam = 55.f + (float)(rand() % 20);

        float margen = obs.tam / 2.f + 40.f;

        obs.pos.x = patioMinX + margen + (float)(rand() % (int)((patioMaxX - patioMinX) - margen * 2.f));
        obs.pos.y = patioMinY + margen + (float)(rand() % (int)((patioMaxY - patioMinY) - margen * 2.f));

        float d1 = std::sqrt(
            std::pow(obs.pos.x - combatiente1.pos.x, 2) +
            std::pow(obs.pos.y - combatiente1.pos.y, 2)
        );

        float d2 = std::sqrt(
            std::pow(obs.pos.x - combatiente2.pos.x, 2) +
            std::pow(obs.pos.y - combatiente2.pos.y, 2)
        );

        if (d1 < 120.f || d2 < 120.f) {
            continue;
        }

        bool demasiadoCerca = false;

        for (const auto& otro : obstaculos) {
            float dist = std::sqrt(
                std::pow(obs.pos.x - otro.pos.x, 2) +
                std::pow(obs.pos.y - otro.pos.y, 2)
            );

            if (dist < (obs.tam / 2.f + otro.tam / 2.f + 60.f)) {
                demasiadoCerca = true;
                break;
            }
        }

        if (!demasiadoCerca) {
            obstaculos.push_back(obs);
        }
    }
}

void Arena::moverCombatiente(CombatienteArena& c, sf::Vector2f dir, float dt) {
    float velocidad = (160.f + c.pieza->velAtaque * 30.f) * c.multiplicadorVelocidad;

    sf::Vector2f nuevaPos = c.pos + dir * velocidad * dt;

    if (dentroDeArena(nuevaPos) && !colisionaConObstaculo(nuevaPos, TAM_PIEZA)) {
        c.pos = nuevaPos;
    }
}

void Arena::crearProyectil(CombatienteArena& tirador, CombatienteArena& objetivo) {
    if (tirador.tiempoRecarga > 0.f) return;

    sf::Vector2f dir = objetivo.pos - tirador.pos;

    float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    if (len < 1.f) return;

    dir /= len;

    Proyectil p;
    p.pos = tirador.pos + dir * (TAM_PIEZA + TAM_PROYECTIL + 2.f);
    p.vel = dir * VEL_PROYECTIL;
    p.activo = true;
    p.esDeLuz = (tirador.pieza->getBando() == LUZ);

    proyectiles.push_back(p);

    tirador.tiempoRecarga = 0.5f;

    std::cout << tirador.pieza->getNombre() << " DISPARA! Proyectiles activos: "
        << proyectiles.size() << std::endl;
}

void Arena::actualizarProyectiles(float dt) {
    for (auto& p : proyectiles) {
        if (!p.activo) continue;

        p.pos += p.vel * dt;

        if (!dentroDeArena(p.pos)) {
            p.activo = false;
            continue;
        }

        if (colisionaConObstaculo(p.pos, TAM_PROYECTIL / 2.f)) {
            p.activo = false;
        }
    }

    proyectiles.erase(
        std::remove_if(
            proyectiles.begin(),
            proyectiles.end(),
            [](const Proyectil& p) {
                return !p.activo;
            }
        ),
        proyectiles.end()
    );
}

void Arena::comprobarColisiones() {
    CombatienteArena& luz = (combatiente1.pieza->getBando() == LUZ) ? combatiente1 : combatiente2;
    CombatienteArena& oscuridad = (combatiente1.pieza->getBando() == LUZ) ? combatiente2 : combatiente1;

    for (auto& p : proyectiles) {
        if (!p.activo) continue;

        if (p.esDeLuz) {
            sf::Vector2f diff = p.pos - oscuridad.pos;

            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (dist < TAM_PIEZA * 1.5f) {
                oscuridad.pieza->vida -= luz.pieza->fuerza * 0.3f;

                if (oscuridad.pieza->vida < 0.f) {
                    oscuridad.pieza->vida = 0.f;
                }

                p.activo = false;

                std::cout << oscuridad.pieza->getNombre() << " golpeado! Vida: "
                    << oscuridad.pieza->vida << std::endl;
            }
        }
        else {
            sf::Vector2f diff = p.pos - luz.pos;

            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (dist < TAM_PIEZA * 1.5f) {
                luz.pieza->vida -= oscuridad.pieza->fuerza * 0.3f;

                if (luz.pieza->vida < 0.f) {
                    luz.pieza->vida = 0.f;
                }

                p.activo = false;

                std::cout << luz.pieza->getNombre() << " golpeado! Vida: "
                    << luz.pieza->vida << std::endl;
            }
        }
    }

    bool muere1 = (combatiente1.pieza->vida <= 0.f);
    bool muere2 = (combatiente2.pieza->vida <= 0.f);

    if (muere1 || muere2) {
        combateTerminado = true;

        if (muere1 && muere2) {
            ganador = nullptr;
        }
        else if (muere2) {
            ganador = combatiente1.pieza;
        }
        else {
            ganador = combatiente2.pieza;
        }
    }
}

bool Arena::colisionaConObstaculo(sf::Vector2f pos, float radio) {
    for (const auto& obs : obstaculos) {
        float dist = std::sqrt(
            std::pow(pos.x - obs.pos.x, 2) +
            std::pow(pos.y - obs.pos.y, 2)
        );

        if (dist < radio + obs.tam / 2.f) {
            return true;
        }
    }

    return false;
}

bool Arena::dentroDeArena(sf::Vector2f pos) {
    float patioMinX = OFFSET_X + MARGEN_PATIO_IZQ;
    float patioMaxX = OFFSET_X + ANCHO - MARGEN_PATIO_DER;
    float patioMinY = OFFSET_Y + MARGEN_PATIO_SUP;
    float patioMaxY = OFFSET_Y + ALTO - MARGEN_PATIO_INF;

    return pos.x >= patioMinX + TAM_PIEZA &&
        pos.x <= patioMaxX - TAM_PIEZA &&
        pos.y >= patioMinY + TAM_PIEZA &&
        pos.y <= patioMaxY - TAM_PIEZA;
}

void Arena::update(float dt) {
    if (combateTerminado) return;

    CombatienteArena& luz = (combatiente1.pieza->getBando() == LUZ) ? combatiente1 : combatiente2;
    CombatienteArena& oscuridad = (combatiente1.pieza->getBando() == LUZ) ? combatiente2 : combatiente1;

    sf::Vector2f dirLuz(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dirLuz.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dirLuz.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dirLuz.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dirLuz.x += 1.f;

    float lenLuz = std::sqrt(dirLuz.x * dirLuz.x + dirLuz.y * dirLuz.y);

    if (lenLuz > 0.f) {
        dirLuz /= lenLuz;
    }

    moverCombatiente(luz, dirLuz, dt);

    bool spaceAhora = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    if (spaceAhora && !luz.teclaDsparoPulsada) {
        crearProyectil(luz, oscuridad);
    }

    luz.teclaDsparoPulsada = spaceAhora;

    sf::Vector2f dirOsc(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) dirOsc.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dirOsc.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) dirOsc.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dirOsc.x += 1.f;

    float lenOsc = std::sqrt(dirOsc.x * dirOsc.x + dirOsc.y * dirOsc.y);

    if (lenOsc > 0.f) {
        dirOsc /= lenOsc;
    }

    moverCombatiente(oscuridad, dirOsc, dt);

    bool enterAhora = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);

    if (enterAhora && !oscuridad.teclaDsparoPulsada) {
        crearProyectil(oscuridad, luz);
    }

    oscuridad.teclaDsparoPulsada = enterAhora;

    if (combatiente1.tiempoRecarga > 0.f) combatiente1.tiempoRecarga -= dt;
    if (combatiente2.tiempoRecarga > 0.f) combatiente2.tiempoRecarga -= dt;

    actualizarProyectiles(dt);
    comprobarColisiones();
}

Pieza* Arena::iniciarCombateAutomatico(Pieza* p1, Pieza* p2) {
    std::cout << "COMBATE AUTOMATICO" << std::endl;

    while (p1->vida > 0 && p2->vida > 0) {
        p1->atacar(p2);

        if (p2->vida > 0) {
            p2->atacar(p1);
        }
    }

    if (p1->vida <= 0 && p2->vida <= 0) {
        return nullptr;
    }
    else if (p1->vida > 0) {
        return p1;
    }
    else {
        return p2;
    }
}

void Arena::setMultiplicadorVelocidad(Pieza* pieza, float multiplicador) {
    if (combatiente1.pieza == pieza) {
        combatiente1.multiplicadorVelocidad = multiplicador;
    }

    if (combatiente2.pieza == pieza) {
        combatiente2.multiplicadorVelocidad = multiplicador;
    }
}