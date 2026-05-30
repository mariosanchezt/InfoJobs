#include "Arena.h"
#include "AudioManager.h"
#include "PiezaMelee.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

static constexpr float MARGEN_PATIO_IZQ = 190.f;
static constexpr float MARGEN_PATIO_DER = 40.f;
static constexpr float MARGEN_PATIO_SUP = 110.f;
static constexpr float MARGEN_PATIO_INF = 105.f;

Arena::Arena() {
    combateTerminado = false;
    ganador = nullptr;
    combatiente1.pieza = nullptr;
    combatiente2.pieza = nullptr;
    audio = nullptr;
    modoIA = false;
}

void Arena::iniciarCombate(Pieza* p1, Pieza* p2) {
    combateTerminado = false;
    ganador = nullptr;
    proyectiles.clear();
    obstaculos.clear();

    float patioMinX = OFFSET_X + MARGEN_PATIO_IZQ;
    float patioMaxX = OFFSET_X + ANCHO - MARGEN_PATIO_DER;
    float patioMinY = OFFSET_Y + MARGEN_PATIO_SUP;
    float patioMaxY = OFFSET_Y + ALTO - MARGEN_PATIO_INF;

    Pieza* piezaLuz = (p1->getBando() == LUZ) ? p1 : p2;
    Pieza* piezaOscuridad = (p1->getBando() == LUZ) ? p2 : p1;

    // Combatiente 1 SIEMPRE es PLANTA / LUZ / IZQUIERDA
    combatiente1.pieza = piezaLuz;
    combatiente1.pos = sf::Vector2f(patioMinX + 75.f, (patioMinY + patioMaxY) / 2.f);
    combatiente1.vel = sf::Vector2f(0.f, 0.f);
    combatiente1.tiempoRecarga = 0.f;
    combatiente1.teclaDsparoPulsada = false;
    combatiente1.multiplicadorVelocidad = 1.f;
    combatiente1.estadoAnim = ANIM_IDLE;
    combatiente1.frameActual = 0;
    combatiente1.timerFrame = 0.f;
    combatiente1.tiempoAtaque = 0.f;

    // Combatiente 2 SIEMPRE es ZOMBIE / OSCURIDAD / DERECHA
    combatiente2.pieza = piezaOscuridad;
    combatiente2.pos = sf::Vector2f(patioMaxX - 75.f, (patioMinY + patioMaxY) / 2.f);
    combatiente2.vel = sf::Vector2f(0.f, 0.f);
    combatiente2.tiempoRecarga = 0.f;
    combatiente2.teclaDsparoPulsada = false;
    combatiente2.multiplicadorVelocidad = 1.f;
    combatiente2.estadoAnim = ANIM_IDLE;
    combatiente2.frameActual = 0;
    combatiente2.timerFrame = 0.f;
    combatiente2.tiempoAtaque = 0.f;

    generarObstaculos();

    std::cout << "=== COMBATE INTERACTIVO ===" << std::endl;
    std::cout << piezaLuz->getNombre() << " (Plantas / WASD + Space) VS "
        << piezaOscuridad->getNombre() << " (Zombies / Flechas + Enter)" << std::endl;
}

void Arena::generarObstaculos() {
    obstaculos.clear();

    float patioMinX = OFFSET_X + MARGEN_PATIO_IZQ;
    float patioMaxX = OFFSET_X + ANCHO - MARGEN_PATIO_DER;
    float patioMinY = OFFSET_Y + MARGEN_PATIO_SUP;
    float patioMaxY = OFFSET_Y + ALTO - MARGEN_PATIO_INF;

    int numObs = 5 + rand() % 3;
    int intentos = 0;

    while ((int)obstaculos.size() < numObs && intentos < 220) {
        intentos++;

        Obstaculo obs;
        obs.tam = 60.f + (float)(rand() % 22);

        float margen = obs.tam / 2.f + 45.f;

        float anchoUtil = (patioMaxX - patioMinX) - margen * 2.f;
        float altoUtil = (patioMaxY - patioMinY) - margen * 2.f;

        if (anchoUtil <= 0.f || altoUtil <= 0.f) break;

        obs.pos.x = patioMinX + margen + (float)(rand() % (int)anchoUtil);
        obs.pos.y = patioMinY + margen + (float)(rand() % (int)altoUtil);

        float d1 = std::sqrt(std::pow(obs.pos.x - combatiente1.pos.x, 2) +
            std::pow(obs.pos.y - combatiente1.pos.y, 2));
        float d2 = std::sqrt(std::pow(obs.pos.x - combatiente2.pos.x, 2) +
            std::pow(obs.pos.y - combatiente2.pos.y, 2));

        if (d1 < 145.f || d2 < 145.f) continue;

        bool demasiadoCerca = false;
        for (const auto& otro : obstaculos) {
            float dist = std::sqrt(std::pow(obs.pos.x - otro.pos.x, 2) +
                std::pow(obs.pos.y - otro.pos.y, 2));
            if (dist < (obs.tam / 2.f + otro.tam / 2.f + 65.f)) {
                demasiadoCerca = true;
                break;
            }
        }

        if (!demasiadoCerca) obstaculos.push_back(obs);
    }
}

void Arena::moverCombatiente(CombatienteArena& c, sf::Vector2f dir, float dt) {
    if (c.pieza == nullptr) return;
    float velocidad = (160.f + c.pieza->velAtaque * 30.f) * c.multiplicadorVelocidad;
    sf::Vector2f nuevaPos = c.pos + dir * velocidad * dt;
    if (dentroDeArena(nuevaPos) && !colisionaConObstaculo(nuevaPos, TAM_PIEZA))
        c.pos = nuevaPos;
}

void Arena::notificarDisparo(CombatienteArena& c) {
    c.estadoAnim = ANIM_ATTACK;
    c.frameActual = 0;
    c.timerFrame = 0.f;
    c.tiempoAtaque = 7 * c.duracionFrame;
}

static bool esMelee(const std::string& nombre) {
    return nombre == "Carnivora" || nombre == "Supercerebroz";
}

void Arena::crearProyectil(CombatienteArena& tirador, CombatienteArena& objetivo) {
    if (tirador.pieza == nullptr || objetivo.pieza == nullptr) return;
    if (tirador.tiempoRecarga > 0.f) return;

    if (esMelee(tirador.pieza->getNombre())) {
        float rango = TAM_PIEZA * 2.8f; // Radio de la onda expansiva

        tirador.tiempoRecarga = 0.8f; // Cooldown más largo para golpes cuerpo a cuerpo
        tirador.tiempoRecargaMax = 0.8f;
        notificarDisparo(tirador);

        // Generar la onda visual SIEMPRE (incluso si ataca al aire)
        OndaMelee onda;
        onda.pos = tirador.pos;
        onda.radioMax = rango;
        onda.duracionMax = 0.25f; // Tiempo que tarda la onda en expandirse y desaparecer
        onda.tiempoVida = onda.duracionMax;
        onda.esDeLuz = (tirador.pieza->getBando() == LUZ);
        ondasMelee.push_back(onda);
        if (audio != nullptr) audio->playMelee();

        // Comprobar si hay un enemigo dentro de la onda
        sf::Vector2f diff = objetivo.pos - tirador.pos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (dist <= rango) {
            objetivo.pieza->vida -= tirador.pieza->fuerza * 0.3f;
            if (objetivo.pieza->vida < 0.f) objetivo.pieza->vida = 0.f;
            if (audio != nullptr)audio->playGolpe();
            std::cout << tirador.pieza->getNombre() << " Muerde a " << objetivo.pieza->getNombre() << "!" << std::endl;
        }
        return;
    }
    tirador.tiempoRecargaMax = 0.5f;

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
    if (audio != nullptr) audio->playDisparo();
    tirador.tiempoRecarga = 0.5f;
    notificarDisparo(tirador);

    std::cout << tirador.pieza->getNombre() << " DISPARA! Proyectiles activos: "
        << proyectiles.size() << std::endl;
}

void Arena::actualizarProyectiles(float dt) {
    for (auto& p : proyectiles) {
        if (!p.activo) continue;
        p.pos += p.vel * dt;
        if (!dentroDeArena(p.pos)) { p.activo = false; continue; }
        if (colisionaConObstaculo(p.pos, TAM_PROYECTIL / 2.f)) p.activo = false;
    }
    proyectiles.erase(
        std::remove_if(proyectiles.begin(), proyectiles.end(),
            [](const Proyectil& p) { return !p.activo; }),
        proyectiles.end()
    );
}

void Arena::comprobarColisiones() {
    if (combatiente1.pieza == nullptr || combatiente2.pieza == nullptr) return;

    CombatienteArena& luz = combatiente1;
    CombatienteArena& oscuridad = combatiente2;

    for (auto& p : proyectiles) {
        if (!p.activo) continue;

        if (p.esDeLuz) {
            sf::Vector2f diff = p.pos - oscuridad.pos;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if (dist < TAM_PIEZA * 1.5f) {
                oscuridad.pieza->vida -= luz.pieza->fuerza * 0.3f;
                if (audio != nullptr) audio->playGolpe();
                if (oscuridad.pieza->vida < 0.f) oscuridad.pieza->vida = 0.f;
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
                if (audio != nullptr) audio->playGolpe();
                if (luz.pieza->vida < 0.f) luz.pieza->vida = 0.f;
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
        if (muere1 && muere2) ganador = nullptr;
        else if (muere2)      ganador = combatiente1.pieza;
        else                  ganador = combatiente2.pieza;
        if (audio != nullptr) audio->playMuerte();
    }
}

bool Arena::colisionaConObstaculo(sf::Vector2f pos, float radio) {
    for (const auto& obs : obstaculos) {
        float dist = std::sqrt(std::pow(pos.x - obs.pos.x, 2) +
            std::pow(pos.y - obs.pos.y, 2));
        if (dist < radio + obs.tam / 2.f) return true;
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
    if (combatiente1.pieza == nullptr || combatiente2.pieza == nullptr) return;

    CombatienteArena& luz = combatiente1;
    CombatienteArena& oscuridad = combatiente2;

    sf::Vector2f dirLuz(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) dirLuz.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) dirLuz.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) dirLuz.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) dirLuz.x += 1.f;

    float lenLuz = std::sqrt(dirLuz.x * dirLuz.x + dirLuz.y * dirLuz.y);
    if (lenLuz > 0.f) dirLuz /= lenLuz;
    moverCombatiente(luz, dirLuz, dt);

    bool spaceAhora = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
    if (spaceAhora && !luz.teclaDsparoPulsada)
        crearProyectil(luz, oscuridad);
    
       
    luz.teclaDsparoPulsada = spaceAhora;

    sf::Vector2f dirOsc(0.f, 0.f);
    if (modoIA) {
        //Moverse hacia el enemigo
        sf::Vector2f diff = luz.pos - oscuridad.pos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        bool esMelee = (dynamic_cast<PiezaMelee*>(oscuridad.pieza) != nullptr);
        float distOptima = esMelee ? 40.f : 150.f;
        float distMinima = esMelee ? 20.f : 100.f;

        if (dist > distOptima) {
            dirOsc = diff / dist; // Avanzar
        }
        else if (dist < distMinima) {
            dirOsc = -diff / dist; // Alejarse
        }

        moverCombatiente(oscuridad, dirOsc, dt);

        //Disparar cuando pueda
        if (oscuridad.tiempoRecarga <= 0.f) {
            crearProyectil(oscuridad, luz);
            if (esMelee) {
                oscuridad.tiempoRecarga = 1.2f;
                oscuridad.tiempoRecargaMax = 1.2f;
            }
        }
    }
    else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    dirOsc.y -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  dirOsc.y += 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  dirOsc.x -= 1.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dirOsc.x += 1.f;

        float lenOsc = std::sqrt(dirOsc.x * dirOsc.x + dirOsc.y * dirOsc.y);
        if (lenOsc > 0.f) dirOsc /= lenOsc;
        moverCombatiente(oscuridad, dirOsc, dt);

        bool enterAhora = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);
        if (enterAhora && !oscuridad.teclaDsparoPulsada)
            crearProyectil(oscuridad, luz);


        oscuridad.teclaDsparoPulsada = enterAhora;
    }

    // Timer de animacion de ambos combatientes
    for (CombatienteArena* c : { &combatiente1, &combatiente2 }) {
        if (c->pieza == nullptr) continue;
        if (c->estadoAnim == ANIM_ATTACK) {
            c->tiempoAtaque -= dt;
            if (c->tiempoAtaque <= 0.f) {
                c->estadoAnim = ANIM_IDLE;
                c->frameActual = 0;
                c->timerFrame = 0.f;
            }
        }
        c->timerFrame += dt;
        if (c->timerFrame >= c->duracionFrame) {
            c->timerFrame = 0.f;
            c->frameActual++;
        }
    }

    if (combatiente1.tiempoRecarga > 0.f) combatiente1.tiempoRecarga -= dt;
    if (combatiente2.tiempoRecarga > 0.f) combatiente2.tiempoRecarga -= dt;
    for (auto& onda : ondasMelee) {
        onda.tiempoVida -= dt;
    }
    ondasMelee.erase(
        std::remove_if(ondasMelee.begin(), ondasMelee.end(),
            [](const OndaMelee& o) { return o.tiempoVida <= 0.f; }),
        ondasMelee.end()
    );

    actualizarProyectiles(dt);
    comprobarColisiones();
}

Pieza* Arena::iniciarCombateAutomatico(Pieza* p1, Pieza* p2) {
    std::cout << "COMBATE AUTOMATICO" << std::endl;
    while (p1->vida > 0 && p2->vida > 0) {
        p1->atacar(p2);
        if (p2->vida > 0) p2->atacar(p1);
    }
    if (p1->vida <= 0 && p2->vida <= 0) return nullptr;
    else if (p1->vida > 0)              return p1;
    else                                return p2;
}

void Arena::setMultiplicadorVelocidad(Pieza* pieza, float multiplicador) {
    if (combatiente1.pieza == pieza) combatiente1.multiplicadorVelocidad = multiplicador;
    if (combatiente2.pieza == pieza) combatiente2.multiplicadorVelocidad = multiplicador;
}