#include "Renderer.h"
#include "Arena.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

Renderer::Renderer(sf::RenderWindow& vent)
    : ventana(vent)
{
    cursorFila = 4;
    cursorCol = 4;
    fuenteCargada = false;
    texturasCargadas = false;
    estado = TABLERO;
    filaSeleccionada = -1;
    colSeleccionada = -1;
    ocultarMunecoCursor = false;
    hechizoSeleccionado = -1;
    modoHechizo = SIN_HECHIZO;
    panelHechizosVisible = false;
    nombrePiezaTeleport = "";

    piezaCongelada = nullptr;
    piezaRalentizada = nullptr;
    piezaFortalecida = nullptr;
    piezaEscudo = nullptr;

    colorBlanco = sf::Color(240, 217, 181);
    colorNegro = sf::Color(90, 120, 80);
    colorGris = sf::Color(160, 160, 140);
    colorSeleccion = sf::Color(255, 215, 0, 180);
    colorMovimiento = sf::Color(80, 200, 80, 140);
}

bool Renderer::cargarFuente(const std::string& ruta) {
    if (!fuente.openFromFile(ruta)) {
        std::cout << "Error: no se pudo cargar la fuente desde " << ruta << std::endl;
        fuenteCargada = false;
        return false;
    }
    fuenteCargada = true;
    return true;
}

std::string Renderer::nombreArchivoSprite(const std::string& nombrePieza) const {
    if (nombrePieza == "Carnivora")      return "Chomper.png";
    if (nombrePieza == "Lanzaguisantes") return "Peashooter.png";
    if (nombrePieza == "Frutaestrella")  return "Starfruit.png";
    if (nombrePieza == "Pomelo")         return "Cactus.png";
    if (nombrePieza == "Mazorca")        return "Cabbage.png";
    if (nombrePieza == "Supercerebroz")  return "Yeti.png";
    if (nombrePieza == "Soldado")        return "Balloon.png";
    if (nombrePieza == "Zombidito")      return "Catapult.png";
    if (nombrePieza == "All-Star")       return "Football.png";
    if (nombrePieza == "Ingeniero")      return "Digger.png";
    if (nombrePieza == "Dave el Loco")   return "Cob_Cannon.png";
    if (nombrePieza == "Dr. Zomboss")    return "Dr. Zomboss.png";
    return "";
}

std::string Renderer::claveAnimacion(const std::string& nombrePieza) const {
    if (nombrePieza == "Carnivora")      return "Chomper";
    if (nombrePieza == "Lanzaguisantes") return "Peashooter";
    if (nombrePieza == "Frutaestrella")  return "Starfruit";
    if (nombrePieza == "Pomelo")         return "Cactus";
    if (nombrePieza == "Mazorca")        return "Cabbage";
    if (nombrePieza == "Supercerebroz")  return "Yeti";
    if (nombrePieza == "Soldado")        return "Balloon";
    if (nombrePieza == "Zombidito")      return "Catapult";
    if (nombrePieza == "All-Star")       return "Football";
    if (nombrePieza == "Ingeniero")      return "Digger";
    if (nombrePieza == "Dave el Loco")   return "Cob_Cannon";
    if (nombrePieza == "Dr. Zomboss")    return "Dr. Zomboss";
    return "";
}

void Renderer::cargarSheetSiNecesario(const std::string& clave, const std::string& carpeta) {
    static const std::map<std::string, int> framesConocidos = {
        { "Peashooter",   8 }, { "Chomper",     9 }, { "Starfruit",   4 },
        { "Cactus",       4 }, { "Cabbage",     6 }, { "Yeti",        9 },
        { "Balloon",      6 }, { "Catapult",    4 }, { "Football",    4 },
        { "Digger",       4 }, { "Cob_Cannon",4 }, { "Dr. Zomboss", 5 }
    };
    static const std::map<std::string, int> framesAttack = {
        { "Chomper",   6 }, { "Cabbage",  8 }, { "Cactus",    2 },
        { "Starfruit", 3 }, { "Yeti",     4 }, { "Catapult",  4 },
        { "Football",  5 }, { "Digger",   5 }, { "Dr. Zomboss",5 }, { "Cob_Cannon",   5 }
    };

    auto getNumFrames = [&](bool esAttack) -> int {
        if (esAttack) {
            auto it = framesAttack.find(clave);
            if (it != framesAttack.end()) return it->second;
        }
        auto it = framesConocidos.find(clave);
        if (it != framesConocidos.end()) return it->second;
        return 1;
        };

    // Detecta offsets reales de cada frame buscando columnas vacias
    auto detectarFrames = [&](sf::Texture& tex, int numFrames)
        -> std::pair<std::vector<int>, std::vector<int>>
        {
            sf::Image img = tex.copyToImage();
            int w = (int)img.getSize().x;
            int h = (int)img.getSize().y;

            // Pa el Chomper tiene negro residual tras createMaskFromColor
            // solo contamos pixeles de color (no negros) como visibles
            bool tieneNegroResidual = (clave == "Chomper");

            std::vector<int> seps;
            for (int x = 0; x < w; x++) {
                int visibles = 0;
                for (int y = 0; y < h; y++) {
                    sf::Color c2 = img.getPixel(sf::Vector2u((unsigned)x, (unsigned)y));
                    if (c2.a > 10) {
                        if (tieneNegroResidual) {
                            if ((int)c2.r + (int)c2.g + (int)c2.b > 60) visibles++;
                        }
                        else {
                            visibles++;
                        }
                    }
                }
                if (visibles <= 1) seps.push_back(x);
            }

            // Agrupar separadores consecutivos
            std::vector<int> grupos;
            if (!seps.empty()) {
                std::vector<int> g = { seps[0] };
                for (int i = 1; i < (int)seps.size(); i++) {
                    if (seps[i] <= g.back() + 5) g.push_back(seps[i]);
                    else { grupos.push_back(g[g.size() / 2]); g = { seps[i] }; }
                }
                grupos.push_back(g[g.size() / 2]);
            }

            // Construir frames entre separadores
            std::vector<std::pair<int, int>> frames;
            int prev = 0;
            for (int sep : grupos) {
                if (sep > prev + 3) frames.push_back({ prev, sep });
                prev = sep + 1;
            }
            if (prev < w) frames.push_back({ prev, w });
            frames.erase(std::remove_if(frames.begin(), frames.end(),
                [](auto& f) { return f.second - f.first < 4; }), frames.end());

            // Si no coincide el numero de frames, usar division uniforme
            if ((int)frames.size() != numFrames) {
                frames.clear();
                int fw = w / numFrames;
                for (int i = 0; i < numFrames; i++)
                    frames.push_back({ i * fw, (i + 1) * fw });
            }

            std::vector<int> offsets, anchos;
            for (auto& [x0, x1] : frames) {
                offsets.push_back(x0); anchos.push_back(x1 - x0);
            }
            return { offsets, anchos };
        };

    auto cargar = [&](std::map<std::string, InfoSheet>& mapa, const std::string& tipo, bool esAttack) {
        if (mapa.count(clave)) return;
        InfoSheet info;
        std::string ruta = carpeta + "/" + clave + "_" + tipo + ".png";

        sf::Image img;
        if (!img.loadFromFile(ruta)) { mapa[clave] = std::move(info); return; }

        // Quitar fondo por color del primer pixel
        if (clave == "Starfruit" || clave == "Chomper") {
            img.createMaskFromColor(img.getPixel(sf::Vector2u(0, 0)));
        }

        // Borrador Inteligente V3
        if (clave == "Chomper") {
            int w = (int)img.getSize().x;
            int h = (int)img.getSize().y;
            for (int x = 0; x < w; x++) {
                int opacos = 0;
                int oscuros = 0;
                for (int y = 0; y < h; y++) {
                    sf::Color c = img.getPixel(sf::Vector2u((unsigned)x, (unsigned)y));
                    if (c.a > 50) {
                        opacos++;
                        if (c.r < 60 && c.g < 60 && c.b < 60) oscuros++;
                    }
                }
                if (opacos > 5 && oscuros >= opacos * 0.6f) {
                    for (int y = 0; y < h; y++) {
                        img.setPixel(sf::Vector2u((unsigned)x, (unsigned)y), sf::Color::Transparent);
                    }
                }
            }
        }

        if (!info.textura.loadFromImage(img)) { mapa[clave] = std::move(info); return; }

        info.textura.setSmooth(false);
        info.altoFrame = (int)info.textura.getSize().y;
        int numFrames = getNumFrames(esAttack);
        auto [offs, anchos] = detectarFrames(info.textura, numFrames);
        info.offsetsX = offs;
        info.anchosFrame = anchos;
        info.numFrames = (int)offs.size();
        info.anchoFrame = *std::max_element(anchos.begin(), anchos.end());
        info.cargada = true;
        std::cout << "Sheet " << tipo << ": " << ruta
            << " (" << info.numFrames << " frames)" << std::endl;
        mapa[clave] = std::move(info);
        };

    cargar(sheetsIdle, "idle", false);
    cargar(sheetsAttack, "attack", true);
}

void Renderer::cargarSprites(const std::string& carpeta) {
    std::vector<std::string> archivos = {
        "Chomper.png", "Peashooter.png", "Starfruit.png", "Cactus.png", "Cabbage.png",
        "Yeti.png", "Balloon.png", "Catapult.png", "Football.png", "Digger.png",
        "Cob_Cannon.png", "Dr. Zomboss.png","Cob_Cannon_proyectil.png",
        "patio.png",
        "Nuez.png",
        "Plantas_Victoria.png",
        "Zombies_Victoria.png",
        "PlantaCara.png", 
        "ZombieCara.png" 
    };

    int cargados = 0;
    for (const auto& archivo : archivos) {
        std::string ruta = carpeta + "/" + archivo;

        sf::Image img;
        if (img.loadFromFile(ruta)) {
            if (archivo == "Starfruit_idle.png" || archivo == "Chomper.png") {
                img.createMaskFromColor(img.getPixel(sf::Vector2u(0, 0)));

                if (archivo == "Chomper.png") {
                    int w = (int)img.getSize().x;
                    int h = (int)img.getSize().y;
                    for (int x = 0; x < w; x++) {
                        int opacos = 0;
                        int oscuros = 0;
                        for (int y = 0; y < h; y++) {
                            sf::Color c = img.getPixel(sf::Vector2u((unsigned)x, (unsigned)y));
                            if (c.a > 50) {
                                opacos++;
                                if (c.r < 60 && c.g < 60 && c.b < 60) oscuros++;
                            }
                        }
                        if (opacos > 5 && oscuros >= opacos * 0.6f) {
                            for (int y = 0; y < h; y++) {
                                img.setPixel(sf::Vector2u((unsigned)x, (unsigned)y), sf::Color::Transparent);
                            }
                        }
                    }
                }
            }

            sf::Texture tex;
            tex.loadFromImage(img);
            tex.setSmooth(true);
            texturas[archivo] = std::move(tex);
            cargados++;
            std::cout << "Sprite cargado: " << archivo << std::endl;
        }
        else {
            std::cout << "Aviso: no se encontro " << ruta << " - usando fallback" << std::endl;
        }
    }

    texturasCargadas = (cargados > 0);
    std::cout << cargados << "/" << archivos.size()
        << " recursos graficos cargados." << std::endl;

    // Proyectiles personalizados por personaje
    std::vector<std::pair<std::string, std::string>> proyectiles = {
        { "Lanzaguisantes", "pea.png" },
        { "Mazorca",        "Cabbage_proyectil.png" },
        { "Pomelo",         "Cactus_proyectil.png" },
        { "Frutaestrella",  "Starfruit_proyectil.png" },
         { "Soldado",        "Balloon_proyectil.png" },      
        { "Zombidito",      "Catapult_proyectil.png" },     
        { "Ingeniero",      "Digger_proyectil.png" },       
        { "All-Star",       "Football_proyectil.png" },
        { "Dave el Loco",   "Cob_Cannon_proyectil.png" },
        {"Dr. Zomboss", "Dr. Zomboss_proyectil.png"}
    };
    for (auto& [nombre, archivo] : proyectiles) {
        sf::Image imgProj;
        if (imgProj.loadFromFile(carpeta + "/" + archivo)) {
            if (nombre == "Frutaestrella") {
                imgProj.createMaskFromColor(imgProj.getPixel(sf::Vector2u(0, 0)));
            }
            sf::Texture tex;
            tex.loadFromImage(imgProj);
            tex.setSmooth(false);
            texturasProyectil[nombre] = std::move(tex);
            std::cout << "Proyectil cargado: " << archivo << std::endl;
        }
    }

    // Pre-cargar sheets de animacion pa todos los personajes
    std::vector<std::string> claves = {
        "Chomper", "Peashooter", "Starfruit", "Cactus", "Cabbage",
        "Yeti", "Balloon", "Catapult", "Football", "Digger",
        "Cob_Cannon", "Dr. Zomboss"
    };
    for (const auto& clave : claves)
        cargarSheetSiNecesario(clave, carpeta);
}

// SISTEMA DE ANIMACIONES

void Renderer::actualizarAnimaciones(float dt) {
    for (auto& anim : animaciones)
        anim.tiempoRestante -= dt;
    animaciones.erase(
        std::remove_if(animaciones.begin(), animaciones.end(),
            [](const AnimHechizo& a) { return a.tiempoRestante <= 0.f; }),
        animaciones.end()
    );
    if (animTeleport.fase != FASE_NINGUNA) {
        animTeleport.progreso += dt / animTeleport.duracionFase;
        if (animTeleport.progreso >= 1.f) {
            animTeleport.progreso = 0.f;
            if (animTeleport.fase == FASE_HUNDIR)
                animTeleport.fase = FASE_MOVER;
            else if (animTeleport.fase == FASE_MOVER)
                animTeleport.fase = FASE_EMERGER;
            else if (animTeleport.fase == FASE_EMERGER)
                animTeleport.fase = FASE_NINGUNA;
        }
    }
}

void Renderer::lanzarAnimHechizo(int fila, int col, TipoAnimHechizo tipo, float duracion) {
    AnimHechizo a;
    a.fila = fila;
    a.col = col;
    a.tipo = tipo;
    a.tiempoRestante = duracion;
    a.tiempoTotal = duracion;
    animaciones.push_back(a);
}

void Renderer::iniciarAnimTeleport(Pieza* pieza, int fOri, int cOri, int fDest, int cDest) {
    animTeleport.fase = FASE_HUNDIR;
    animTeleport.pieza = pieza;
    animTeleport.filaOrigen = fOri;
    animTeleport.colOrigen = cOri;
    animTeleport.filaDest = fDest;
    animTeleport.colDest = cDest;
    animTeleport.progreso = 0.f;
    animTeleport.duracionFase = 0.3f;
}

void Renderer::dibujarBordeEfecto(int fila, int col, sf::Color color, float t) {
    float pulso = 0.5f + 0.5f * std::sin(t * 3.f);
    color.a = static_cast<uint8_t>(150 + static_cast<int>(105 * pulso));

    sf::RectangleShape borde(sf::Vector2f(TAM_CASILLA - 4.f, TAM_CASILLA - 4.f));
    borde.setPosition(sf::Vector2f(
        OFFSET_X + col * TAM_CASILLA + 2.f,
        OFFSET_Y + fila * TAM_CASILLA + 2.f
    ));
    borde.setFillColor(sf::Color::Transparent);
    borde.setOutlineColor(color);
    borde.setOutlineThickness(3.f);
    ventana.draw(borde);

    sf::Color colorInterior = color;
    colorInterior.a = static_cast<uint8_t>(80 + static_cast<int>(80 * pulso));
    sf::RectangleShape bordeInterior(sf::Vector2f(TAM_CASILLA - 14.f, TAM_CASILLA - 14.f));
    bordeInterior.setPosition(sf::Vector2f(
        OFFSET_X + col * TAM_CASILLA + 7.f,
        OFFSET_Y + fila * TAM_CASILLA + 7.f
    ));
    bordeInterior.setFillColor(sf::Color::Transparent);
    bordeInterior.setOutlineColor(colorInterior);
    bordeInterior.setOutlineThickness(2.f);
    ventana.draw(bordeInterior);

    float radio = 7.f + 2.f * pulso;
    sf::CircleShape marca(radio);
    marca.setFillColor(color);
    marca.setOutlineColor(sf::Color::White);
    marca.setOutlineThickness(1.f);
    marca.setOrigin(sf::Vector2f(radio, radio));
    marca.setPosition(sf::Vector2f(
        OFFSET_X + col * TAM_CASILLA + TAM_CASILLA - 13.f,
        OFFSET_Y + fila * TAM_CASILLA + 13.f
    ));
    ventana.draw(marca);
}

void Renderer::dibujarAnimaciones() {
    for (const auto& anim : animaciones) {
        float progreso = 1.f - (anim.tiempoRestante / anim.tiempoTotal);
        float cx = OFFSET_X + anim.col * TAM_CASILLA + TAM_CASILLA / 2.f;
        float cy = OFFSET_Y + anim.fila * TAM_CASILLA + TAM_CASILLA / 2.f;

        uint8_t alpha = static_cast<uint8_t>(220 * (1.f - progreso));
        float radio = TAM_CASILLA * 0.18f + TAM_CASILLA * 0.42f * progreso;

        sf::Color colorFlash;
        switch (anim.tipo) {
        case ANIM_CURACION:   colorFlash = sf::Color(50, 220, 80, alpha);  break;
        case ANIM_DANO:       colorFlash = sf::Color(220, 50, 50, alpha);  break;
        case ANIM_RALENTIZAR: colorFlash = sf::Color(220, 180, 30, alpha); break;
        case ANIM_FORTALECER: colorFlash = sf::Color(255, 200, 0, alpha);  break;
        case ANIM_ESCUDO:     colorFlash = sf::Color(80, 160, 255, alpha); break;
        case ANIM_CONGELAR:   colorFlash = sf::Color(120, 220, 255, alpha); break;
        default: continue;
        }

        sf::CircleShape circulo(radio);
        circulo.setFillColor(sf::Color::Transparent);
        circulo.setOutlineColor(colorFlash);
        circulo.setOutlineThickness(3.f);
        circulo.setOrigin(sf::Vector2f(radio, radio));
        circulo.setPosition(sf::Vector2f(cx, cy));
        ventana.draw(circulo);

        float radio2 = radio * 0.55f;
        sf::CircleShape circulo2(radio2);
        circulo2.setFillColor(sf::Color(colorFlash.r, colorFlash.g, colorFlash.b, alpha / 3));
        circulo2.setOrigin(sf::Vector2f(radio2, radio2));
        circulo2.setPosition(sf::Vector2f(cx, cy));
        ventana.draw(circulo2);

        const int numParticulas = 8;
        const float PI2 = 6.28318f;
        for (int i = 0; i < numParticulas; i++) {
            float angulo = (PI2 / numParticulas) * i;
            float distancia = 12.f + 32.f * progreso;
            float px = cx + std::cos(angulo) * distancia;
            float py = cy + std::sin(angulo) * distancia;
            float tam = 3.f + 3.f * (1.f - progreso);
            sf::CircleShape particula(tam);
            particula.setFillColor(colorFlash);
            particula.setOrigin(sf::Vector2f(tam, tam));
            particula.setPosition(sf::Vector2f(px, py));
            ventana.draw(particula);
        }
    }
}

void Renderer::dibujarTeleportEnCurso() {
    if (animTeleport.fase == FASE_NINGUNA || animTeleport.pieza == nullptr) return;
    float p = animTeleport.progreso;

    if (animTeleport.fase == FASE_HUNDIR) {
        float escala = (1.f - p) * (1.f + 0.15f * std::sin(p * 3.14159f));
        if (escala < 0.f) escala = 0.f;
        sf::Vector2f centro = getCentroCasilla(animTeleport.filaOrigen, animTeleport.colOrigen);
        centro.y += TAM_CASILLA * 0.3f * p;
        dibujarPiezaPixelEscala(animTeleport.pieza, centro.x, centro.y, escala);
    }
    else if (animTeleport.fase == FASE_MOVER) {
        // nada visible
    }
    else if (animTeleport.fase == FASE_EMERGER) {
        float escala = p * (1.f + 0.15f * std::sin((1.f - p) * 3.14159f));
        if (escala > 1.f) escala = 1.f;
        sf::Vector2f centro = getCentroCasilla(animTeleport.filaDest, animTeleport.colDest);
        centro.y += TAM_CASILLA * 0.3f * (1.f - p);
        dibujarPiezaPixelEscala(animTeleport.pieza, centro.x, centro.y, escala);
    }
}

// TABLERO

void Renderer::dibujarEstadoTablero(
    Tablero* tablero, Bando turno,
    bool* hechizosUsadosLuz, bool* hechizosUsadosOscuridad,
    int hechizoSeleccionado,
    const std::map<std::string, int>& cementerioLuz,
    const std::map<std::string, int>& cementerioOscuridad
) {
    if (tablero == nullptr) return;
    dibujarTablero(tablero);
    dibujarPuntosDePoder(tablero);
    dibujarMovimientosDisponibles();

    if (filaSeleccionada >= 0 && colSeleccionada >= 0)
        dibujarCasillaResaltada(filaSeleccionada, colSeleccionada);

    float t = relojAnimacion.getElapsedTime().asSeconds();

    if (piezaCongelada != nullptr)
        dibujarBordeEfecto(piezaCongelada->filaInicial, piezaCongelada->colInicial, sf::Color(120, 220, 255), t);
    if (piezaRalentizada != nullptr)
        dibujarBordeEfecto(piezaRalentizada->filaInicial, piezaRalentizada->colInicial, sf::Color(220, 180, 30), t);
    if (piezaFortalecida != nullptr)
        dibujarBordeEfecto(piezaFortalecida->filaInicial, piezaFortalecida->colInicial, sf::Color(255, 200, 0), t);
    if (piezaEscudo != nullptr)
        dibujarBordeEfecto(piezaEscudo->filaInicial, piezaEscudo->colInicial, sf::Color(60, 120, 255), t);

    dibujarAnimaciones();
    dibujarTeleportEnCurso();
    dibujarCursor(tablero);

    Pieza* piezaSeleccionada = nullptr;
    if (filaSeleccionada >= 0 && colSeleccionada >= 0)
        piezaSeleccionada = tablero->getPieza(filaSeleccionada, colSeleccionada);

    dibujarIndicadorTurno(turno, piezaSeleccionada, tablero, hechizoSeleccionado);

    if (panelHechizosVisible)
        dibujarPanelHechizos(turno, hechizosUsadosLuz, hechizosUsadosOscuridad, hechizoSeleccionado);


    dibujarCementerio(cementerioLuz, CEMENTERIO_X_LUZ, CEMENTERIO_Y, sf::Color(80, 220, 120), "CEMENTERIO PLANTAS");
    dibujarCementerio(cementerioOscuridad, CEMENTERIO_X_OSCURIDAD, CEMENTERIO_Y, sf::Color(220, 80, 80), "CEMENTERIO ZOMBIES");

    dibujarPanelPoderBando(tablero, LUZ, CEMENTERIO_X_LUZ, PODER_PANEL_Y, sf::Color(80, 220, 120), "PODER PLANTAS");
    dibujarPanelPoderBando(tablero, OSCURIDAD, CEMENTERIO_X_OSCURIDAD, PODER_PANEL_Y, sf::Color(220, 80, 80), "PODER ZOMBIES");
}

void Renderer::dibujarTablero(Tablero* tablero) {
    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            int colorActual = tablero->getColorActual(f, c);
            sf::Color colorFondo;
            if (colorActual == 0) colorFondo = colorBlanco;
            else if (colorActual == 1) colorFondo = colorNegro;
            else colorFondo = colorGris;

            sf::RectangleShape casilla(sf::Vector2f(TAM_CASILLA, TAM_CASILLA));
            casilla.setPosition(sf::Vector2f(OFFSET_X + c * TAM_CASILLA, OFFSET_Y + f * TAM_CASILLA));
            casilla.setFillColor(colorFondo);
            casilla.setOutlineColor(sf::Color(50u, 50u, 50u));
            casilla.setOutlineThickness(1.f);
            ventana.draw(casilla);

            Pieza* p = tablero->getPieza(f, c);
            if (p != nullptr) {
                if (ocultarMunecoCursor && f == filaSeleccionada && c == colSeleccionada)
                    continue;
                if (animTeleport.fase != FASE_NINGUNA && p == animTeleport.pieza)
                    continue;
                dibujarPieza(p, f, c);
            }
        }
    }
}

void Renderer::dibujarPieza(Pieza* p, int fila, int col) {
    float px = OFFSET_X + col * TAM_CASILLA;
    float py = OFFSET_Y + fila * TAM_CASILLA;
    float margen = 6.f;
    float tamDisp = TAM_CASILLA - margen * 2.f;
    bool dibujado = false;

    // Usar sheet idle animada si existe
    std::string clave = claveAnimacion(p->getNombre());
    if (!clave.empty() && sheetsIdle.count(clave) > 0 && sheetsIdle.at(clave).cargada) {
        const InfoSheet& sheet = sheetsIdle.at(clave);
        float t = relojAnimacion.getElapsedTime().asSeconds();
        int frameIdx = (int)(t / 0.15f) % sheet.numFrames;
        int offX = sheet.offsetsX.empty() ? frameIdx * sheet.anchoFrame : sheet.offsetsX[frameIdx];
        int anchof = sheet.anchosFrame.empty() ? sheet.anchoFrame : sheet.anchosFrame[frameIdx];

        sf::Sprite sprite(sheet.textura);
        sprite.setTextureRect(sf::IntRect(
            sf::Vector2i(offX, 0),
            sf::Vector2i(anchof, sheet.altoFrame)
        ));
        float escala = tamDisp / std::max((float)sheet.anchoFrame, (float)sheet.altoFrame);
        float spriteW = anchof * escala;
        float spriteH = sheet.altoFrame * escala;
        sprite.setScale(sf::Vector2f(escala, escala));
        sprite.setPosition(sf::Vector2f(
            px + (TAM_CASILLA - spriteW) / 2.f,
            py + (TAM_CASILLA - spriteH) / 2.f
        ));
        ventana.draw(sprite);
        dibujado = true;
    }

    // Fallback: sprite estatico
    if (!dibujado) {
        std::string archivo = nombreArchivoSprite(p->getNombre());
        bool tieneSprite = !archivo.empty() && texturas.count(archivo) > 0;

        if (tieneSprite) {
            const sf::Texture& tex = texturas.at(archivo);
            sf::Sprite sprite(tex);
            sf::Vector2u texSize = tex.getSize();
            float escala = tamDisp / std::max((float)texSize.x, (float)texSize.y);
            sprite.setScale(sf::Vector2f(escala, escala));
            float spriteW = texSize.x * escala;
            float spriteH = texSize.y * escala;
            sprite.setPosition(sf::Vector2f(
                px + (TAM_CASILLA - spriteW) / 2.f,
                py + (TAM_CASILLA - spriteH) / 2.f
            ));
            ventana.draw(sprite);
        }
        else {
            sf::Color colorPieza = (p->getBando() == LUZ)
                ? sf::Color(80, 180, 100)
                : sf::Color(180, 60, 60);
            float radio = TAM_CASILLA * 0.35f;
            sf::CircleShape circulo(radio);
            circulo.setFillColor(colorPieza);
            circulo.setOutlineColor(sf::Color::Black);
            circulo.setOutlineThickness(2.f);
            circulo.setPosition(sf::Vector2f(px + TAM_CASILLA / 2.f - radio, py + TAM_CASILLA / 2.f - radio - 8.f));
            ventana.draw(circulo);
        }
    }

    dibujarBarraVida(p->vida, p->vidaMaxima, px + 5.f, py + 4.f, TAM_CASILLA - 10.f);
}

void Renderer::dibujarCasillaResaltada(int fila, int col) {
    float t = relojAnimacion.getElapsedTime().asSeconds();
    float pulso = 0.5f + 0.5f * std::sin(t * 4.f);
    uint8_t alpha = static_cast<uint8_t>(180 + static_cast<int>(75 * pulso));

    sf::Color colorResalte(255u, 215u, 0u, alpha);
    if (filaSeleccionada >= 0 && colSeleccionada >= 0) {
        colorResalte = sf::Color(60u, 255u, 60u, alpha);
        if (col >= 7) colorResalte = sf::Color(255u, 60u, 60u, alpha);
    }

    sf::RectangleShape resalte(sf::Vector2f(TAM_CASILLA - 4.f, TAM_CASILLA - 4.f));
    resalte.setPosition(sf::Vector2f(OFFSET_X + col * TAM_CASILLA + 2.f, OFFSET_Y + fila * TAM_CASILLA + 2.f));
    resalte.setFillColor(sf::Color::Transparent);
    resalte.setOutlineColor(colorResalte);
    resalte.setOutlineThickness(4.f);
    ventana.draw(resalte);
}

void Renderer::dibujarMovimientosDisponibles() {
    for (auto& pos : movimientosDisponibles) {
        int f = pos.first;
        int c = pos.second;

        sf::RectangleShape marca(sf::Vector2f(TAM_CASILLA, TAM_CASILLA));
        marca.setPosition(sf::Vector2f(OFFSET_X + c * TAM_CASILLA, OFFSET_Y + f * TAM_CASILLA));
        marca.setFillColor(colorMovimiento);
        marca.setOutlineColor(sf::Color(50u, 180u, 50u));
        marca.setOutlineThickness(2.f);
        ventana.draw(marca);

        float radio = TAM_CASILLA * 0.18f;
        sf::CircleShape punto(radio);
        punto.setFillColor(sf::Color(30u, 160u, 30u, 200u));
        punto.setPosition(sf::Vector2f(
            OFFSET_X + c * TAM_CASILLA + TAM_CASILLA / 2.f - radio,
            OFFSET_Y + f * TAM_CASILLA + TAM_CASILLA / 2.f - radio
        ));
        ventana.draw(punto);
    }
}

void Renderer::dibujarBarraVida(float vida, float vidaMax, float x, float y, float ancho) {
    if (vidaMax <= 0) return;
    float porcentaje = vida / vidaMax;

    sf::RectangleShape fondo(sf::Vector2f(ancho, 5.f));
    fondo.setPosition(sf::Vector2f(x, y));
    fondo.setFillColor(sf::Color(30, 30, 30, 180));
    ventana.draw(fondo);

    sf::Color colorVida;
    if (porcentaje > 0.6f)      colorVida = sf::Color(50, 200, 50);
    else if (porcentaje > 0.3f) colorVida = sf::Color(230, 180, 30);
    else                        colorVida = sf::Color(220, 50, 50);

    sf::RectangleShape relleno(sf::Vector2f(ancho * porcentaje, 5.f));
    relleno.setPosition(sf::Vector2f(x, y));
    relleno.setFillColor(colorVida);
    ventana.draw(relleno);
}
void Renderer::dibujarBarraCooldown(float tiempoRecarga, float tiempoRecargaMax, float x, float y, float ancho) {
    if (tiempoRecargaMax <= 0.f) return;

    // Si ya puede atacar, dibujamos una rayita fina color azul
    if (tiempoRecarga <= 0.f) {
        sf::RectangleShape lista(sf::Vector2f(ancho, 3.f));
        lista.setPosition(sf::Vector2f(x, y));
        lista.setFillColor(sf::Color(100, 255, 255, 200));
        ventana.draw(lista);
        return;
    }

    // Si está recargando, barra naranja que se va llenando
    float porcentaje = 1.f - (tiempoRecarga / tiempoRecargaMax);

    sf::RectangleShape fondo(sf::Vector2f(ancho, 3.f));
    fondo.setPosition(sf::Vector2f(x, y));
    fondo.setFillColor(sf::Color(30, 30, 30, 180));
    ventana.draw(fondo);

    sf::RectangleShape relleno(sf::Vector2f(ancho * porcentaje, 3.f));
    relleno.setPosition(sf::Vector2f(x, y));
    relleno.setFillColor(sf::Color(255, 150, 50));
    ventana.draw(relleno);
}

void Renderer::dibujarIndicadorTurno(Bando turno, Pieza* piezaSeleccionada, Tablero* tablero, int hechizoSeleccionado) {
    float yFranja = OFFSET_Y + 9.f * TAM_CASILLA + 5.f;
    bool esLuz = (turno == LUZ);

    sf::Color colorFranja = esLuz ? sf::Color(50, 130, 70) : sf::Color(130, 40, 40);
    if (modoHechizo == HECHIZO_ALIADO)  colorFranja = sf::Color(30, 80, 160);
    if (modoHechizo == HECHIZO_ENEMIGO) colorFranja = sf::Color(160, 60, 20);
    if (modoHechizo == HECHIZO_CASILLA) colorFranja = sf::Color(100, 30, 130);

    sf::RectangleShape franja(sf::Vector2f(760.f, 50.f));
    franja.setPosition(sf::Vector2f(OFFSET_X, yFranja));
    franja.setFillColor(colorFranja);
    ventana.draw(franja);

    if (!fuenteCargada) return;

    std::string textoInferior;
    if (modoHechizo == HECHIZO_ALIADO) {
        textoInferior = "Selecciona una pieza ALIADA como objetivo  |  Esc: cancelar";
    }
    else if (modoHechizo == HECHIZO_ENEMIGO) {
        textoInferior = "Selecciona una pieza ENEMIGA como objetivo  |  Esc: cancelar";
    }
    else if (modoHechizo == HECHIZO_CASILLA) {
        if (animTeleport.fase != FASE_NINGUNA)
            textoInferior = "Teletransportando...";
        else if (!nombrePiezaTeleport.empty())
            textoInferior = nombrePiezaTeleport + " seleccionado  |  Muevelo a la casilla de destino  |  Esc: cancelar";
        else
            textoInferior = "Selecciona la pieza que quieres TELETRANSPORTAR  |  Esc: cancelar";
    }
    else if (piezaSeleccionada != nullptr) {
        textoInferior = construirTextoPieza(piezaSeleccionada);
    }
    else {
        if (tablero != nullptr) {
            Pieza* bajoCursor = tablero->getPieza(cursorFila, cursorCol);
            if (bajoCursor != nullptr && bajoCursor->getBando() == turno)
                textoInferior = bajoCursor->getNombre() + "  |  Space/Enter: seleccionar  |  H: hechizos";
            else if (bajoCursor != nullptr)
                textoInferior = construirTextoPieza(bajoCursor);
            else
                textoInferior = esLuz ? "TURNO: PLANTAS  |  H: abrir hechizos" : "TURNO: ZOMBIES  |  H: abrir hechizos";
        }
        else {
            textoInferior = esLuz ? "TURNO: PLANTAS" : "TURNO: ZOMBIES";
        }
    }

    sf::Text texto(fuente, textoInferior, 20);
    texto.setFillColor(sf::Color::White);
    texto.setStyle(sf::Text::Bold);
    sf::FloatRect b = texto.getLocalBounds();
    texto.setPosition(sf::Vector2f(OFFSET_X + HUD_ANCHO / 2.f - b.size.x / 2.f, yFranja + 15.f));
    ventana.draw(texto);
}

std::string Renderer::tipoMovimientoTexto(TipoMovimiento mov) const {
    switch (mov) {
    case GROUND:   return "GROUND";
    case FLYING:   return "FLYING";
    case TELEPORT: return "TELEPORT";
    default:       return "UNKNOWN";
    }
}

std::string Renderer::numeroTexto(float valor) const {
    std::ostringstream oss;
    if (std::fabs(valor - std::round(valor)) < 0.01f)
        oss << static_cast<int>(std::round(valor));
    else
        oss << std::fixed << std::setprecision(1) << valor;
    return oss.str();
}

std::string Renderer::construirTextoPieza(Pieza* pieza) const {
    if (pieza == nullptr) return "";
    std::ostringstream oss;
    oss << pieza->getNombre()
        << "  VIDA: " << numeroTexto(pieza->vida) << "/" << numeroTexto(pieza->vidaMaxima)
        << "  FUERZA: " << numeroTexto(pieza->fuerza)
        << "  MOV: " << pieza->radioMovimiento
        << "  " << tipoMovimientoTexto(pieza->mov);
    return oss.str();
}

void Renderer::dibujarCursor(Tablero* tablero) {
    sf::Color colorCursor;
    if (modoHechizo == HECHIZO_ALIADO)
        colorCursor = sf::Color(80, 120, 255, 220);
    else if (modoHechizo == HECHIZO_ENEMIGO)
        colorCursor = sf::Color(255, 100, 30, 220);
    else if (modoHechizo == HECHIZO_CASILLA)
        colorCursor = sf::Color(200, 80, 255, 220);
    else if (filaSeleccionada >= 0 && colSeleccionada >= 0 && tablero != nullptr) {
        Pieza* p = tablero->getPieza(filaSeleccionada, colSeleccionada);
        colorCursor = (p != nullptr)
            ? ((p->getBando() == LUZ) ? sf::Color(40, 255, 40, 220) : sf::Color(255, 60, 60, 220))
            : sf::Color(0, 255, 255, 200);
    }
    else {
        colorCursor = sf::Color(0, 255, 255, 200);
    }

    sf::RectangleShape cursor(sf::Vector2f(TAM_CASILLA, TAM_CASILLA));
    cursor.setPosition(sf::Vector2f(OFFSET_X + cursorCol * TAM_CASILLA, OFFSET_Y + cursorFila * TAM_CASILLA));
    cursor.setFillColor(sf::Color::Transparent);
    cursor.setOutlineColor(colorCursor);
    cursor.setOutlineThickness(4.f);
    ventana.draw(cursor);
}



void Renderer::dibujarSpriteCementerio(
    const std::string& nombrePieza,
    float x,
    float y,
    float tam
) {
    std::string archivo = nombreArchivoSprite(nombrePieza);

    if (!archivo.empty() && texturas.count(archivo) > 0) {
        const sf::Texture& tex = texturas.at(archivo);
        sf::Sprite sprite(tex);

        sf::Vector2u texSize = tex.getSize();

        float escala = tam / std::max((float)texSize.x, (float)texSize.y);

        sprite.setScale(sf::Vector2f(escala, escala));

        float spriteW = texSize.x * escala;
        float spriteH = texSize.y * escala;

        sprite.setPosition(sf::Vector2f(
            x + (tam - spriteW) / 2.f,
            y + (tam - spriteH) / 2.f
        ));

        ventana.draw(sprite);
    }
    else {
        sf::CircleShape icono(tam / 2.f);
        icono.setFillColor(sf::Color(120u, 120u, 120u));
        icono.setOutlineColor(sf::Color::White);
        icono.setOutlineThickness(1.f);
        icono.setPosition(sf::Vector2f(x, y));
        ventana.draw(icono);
    }
}

void Renderer::dibujarCementerio(
    const std::map<std::string, int>& cementerio,
    float x,
    float y,
    sf::Color colorTitulo,
    const std::string& titulo
) {
    sf::RectangleShape panel(sf::Vector2f(CEMENTERIO_ANCHO, CEMENTERIO_ALTO));
    panel.setPosition(sf::Vector2f(x, y));
    panel.setFillColor(sf::Color(20, 20, 20, 210));
    panel.setOutlineColor(colorTitulo);
    panel.setOutlineThickness(3.f);
    ventana.draw(panel);

    if (!fuenteCargada) return;

    sf::Text tituloTxt(fuente, titulo, 15);
    tituloTxt.setFillColor(colorTitulo);
    tituloTxt.setStyle(sf::Text::Bold);

    sf::FloatRect bt = tituloTxt.getLocalBounds();
    tituloTxt.setPosition(sf::Vector2f(
        x + CEMENTERIO_ANCHO / 2.f - bt.size.x / 2.f,
        y + 12.f
    ));
    ventana.draw(tituloTxt);

    std::string iconoArchivo = "ZombieCara.png";

    if (titulo.find("PLANTAS") != std::string::npos) {
        iconoArchivo = "PlantaCara.png";
    }

    float tamIcono = 44.f;
    float iconoY = y + 37.f;

    if (texturas.count(iconoArchivo) > 0) {
        const sf::Texture& tex = texturas.at(iconoArchivo);
        sf::Sprite iconoSprite(tex);

        sf::Vector2u texSize = tex.getSize();
        float escala = tamIcono / std::max((float)texSize.x, (float)texSize.y);

        iconoSprite.setScale(sf::Vector2f(escala, escala));

        float spriteW = texSize.x * escala;
        float spriteH = texSize.y * escala;

        iconoSprite.setPosition(sf::Vector2f(
            x + CEMENTERIO_ANCHO / 2.f - spriteW / 2.f,
            iconoY + (tamIcono - spriteH) / 2.f
        ));

        ventana.draw(iconoSprite);
    }
    else {
        sf::Text iconoFallback(fuente, titulo.find("PLANTAS") != std::string::npos ? "P" : "Z", 26);
        iconoFallback.setFillColor(colorTitulo);
        iconoFallback.setStyle(sf::Text::Bold);

        sf::FloatRect bi = iconoFallback.getLocalBounds();
        iconoFallback.setPosition(sf::Vector2f(
            x + CEMENTERIO_ANCHO / 2.f - bi.size.x / 2.f,
            y + 43.f
        ));
        ventana.draw(iconoFallback);
    }

    sf::RectangleShape separador(sf::Vector2f(CEMENTERIO_ANCHO - 24.f, 2.f));
    separador.setPosition(sf::Vector2f(x + 12.f, y + 82.f));
    separador.setFillColor(sf::Color(colorTitulo.r, colorTitulo.g, colorTitulo.b, 160u));
    ventana.draw(separador);

    if (cementerio.empty()) {
        sf::Text vacio(fuente, "Sin bajas", 16);
        vacio.setFillColor(sf::Color(180, 180, 180));

        sf::FloatRect bv = vacio.getLocalBounds();
        vacio.setPosition(sf::Vector2f(
            x + CEMENTERIO_ANCHO / 2.f - bv.size.x / 2.f,
            y + 115.f
        ));

        ventana.draw(vacio);
        return;
    }

    float tamSprite = 72.f;
    float celdaW = 94.f;
    float celdaH = 86.f;

    float inicioX = x + 16.f;
    float inicioY = y + 100.f;

    int indice = 0;
    int maxItems = 8;

    for (const auto& par : cementerio) {
        if (indice >= maxItems) {
            sf::Text mas(fuente, "...", 22);
            mas.setFillColor(sf::Color::White);
            mas.setStyle(sf::Text::Bold);
            mas.setPosition(sf::Vector2f(x + CEMENTERIO_ANCHO / 2.f - 10.f, y + CEMENTERIO_ALTO - 36.f));
            ventana.draw(mas);
            break;
        }

        int columna = indice % 2;
        int fila = indice / 2;

        float celdaX = inicioX + columna * celdaW;
        float celdaY = inicioY + fila * celdaH;

        sf::RectangleShape fondoCelda(sf::Vector2f(82.f, 78.f));
        fondoCelda.setPosition(sf::Vector2f(celdaX - 5.f, celdaY - 3.f));
        fondoCelda.setFillColor(sf::Color(40u, 40u, 40u, 145u));
        fondoCelda.setOutlineColor(sf::Color(90u, 90u, 90u, 130u));
        fondoCelda.setOutlineThickness(1.f);
        ventana.draw(fondoCelda);

        dibujarSpriteCementerio(par.first, celdaX, celdaY, tamSprite);

        sf::CircleShape chapa(17.f);
        chapa.setFillColor(sf::Color(0u, 0u, 0u, 210u));
        chapa.setOutlineColor(colorTitulo);
        chapa.setOutlineThickness(2.f);
        chapa.setPosition(sf::Vector2f(celdaX + 48.f, celdaY + 48.f));
        ventana.draw(chapa);

        std::string textoCantidad = "x" + std::to_string(par.second);

        sf::Text cantidad(fuente, textoCantidad, 15);
        cantidad.setFillColor(sf::Color::White);
        cantidad.setStyle(sf::Text::Bold);

        sf::FloatRect bc = cantidad.getLocalBounds();
        cantidad.setPosition(sf::Vector2f(
            celdaX + 65.f - bc.size.x / 2.f,
            celdaY + 55.f - bc.size.y / 2.f
        ));
        ventana.draw(cantidad);

        indice++;
    }
}

int Renderer::contarPuntosPoder(Tablero* tablero, Bando bando) const {
    if (tablero == nullptr) return 0;
    const std::pair<int, int> puntos[5] = { {0,4}, {4,0}, {4,4}, {4,8}, {8,4} };
    int contador = 0;
    for (const auto& punto : puntos) {
        Pieza* p = tablero->getPieza(punto.first, punto.second);
        if (p != nullptr && p->getBando() == bando) contador++;
    }
    return contador;
}

void Renderer::dibujarPanelPoderBando(
    Tablero* tablero, Bando bando,
    float x, float y,
    sf::Color colorTitulo,
    const std::string& titulo
) {
    if (!fuenteCargada || tablero == nullptr) return;
    int puntos = contarPuntosPoder(tablero, bando);

    sf::RectangleShape panel(sf::Vector2f(CEMENTERIO_ANCHO, PODER_PANEL_ALTO));
    panel.setPosition(sf::Vector2f(x, y));
    panel.setFillColor(sf::Color(20, 20, 20, 210));
    panel.setOutlineColor(colorTitulo);
    panel.setOutlineThickness(3.f);
    ventana.draw(panel);

    sf::Text tituloTxt(fuente, titulo, 15);
    tituloTxt.setFillColor(colorTitulo);
    tituloTxt.setStyle(sf::Text::Bold);
    sf::FloatRect bt = tituloTxt.getLocalBounds();
    tituloTxt.setPosition(sf::Vector2f(x + CEMENTERIO_ANCHO / 2.f - bt.size.x / 2.f, y + 12.f));
    ventana.draw(tituloTxt);

    std::string textoConteo = std::to_string(puntos) + " de 5";
    sf::Text conteo(fuente, textoConteo, 20);
    conteo.setFillColor(sf::Color::White);
    conteo.setStyle(sf::Text::Bold);
    sf::FloatRect bc = conteo.getLocalBounds();
    conteo.setPosition(sf::Vector2f(x + CEMENTERIO_ANCHO / 2.f - bc.size.x / 2.f, y + 43.f));
    ventana.draw(conteo);

    dibujarCirculosPoder(puntos, x + 21.f, y + 82.f, colorTitulo);
}

void Renderer::dibujarCirculosPoder(int puntosControlados, float x, float y, sf::Color colorActivo) {
    float radio = 11.f;
    float separacion = 37.f;
    for (int i = 0; i < 5; i++) {
        sf::CircleShape circulo(radio);
        circulo.setPosition(sf::Vector2f(x + i * separacion, y));
        if (i < puntosControlados) {
            circulo.setFillColor(colorActivo);
            circulo.setOutlineColor(sf::Color::White);
        }
        else {
            circulo.setFillColor(sf::Color(60, 60, 60));
            circulo.setOutlineColor(sf::Color(130, 130, 130));
        }
        circulo.setOutlineThickness(2.f);
        ventana.draw(circulo);
    }
}

// ARENA

void Renderer::dibujarEstadoArena(const Arena& arena) {
    bool tienePatio = texturas.count("patio.png") > 0;
    if (tienePatio) {
        const sf::Texture& tex = texturas.at("patio.png");
        sf::Sprite fondo(tex);
        sf::Vector2u texSize = tex.getSize();
        fondo.setScale(sf::Vector2f(Arena::getAncho() / (float)texSize.x, Arena::getAlto() / (float)texSize.y));
        fondo.setPosition(sf::Vector2f(Arena::getOffsetX(), Arena::getOffsetY()));
        ventana.draw(fondo);
    }
    else {
        sf::RectangleShape fondo(sf::Vector2f(Arena::getAncho(), Arena::getAlto()));
        fondo.setPosition(sf::Vector2f(Arena::getOffsetX(), Arena::getOffsetY()));
        fondo.setFillColor(sf::Color(50, 120, 55));
        ventana.draw(fondo);
    }

    sf::RectangleShape borde(sf::Vector2f(Arena::getAncho(), Arena::getAlto()));
    borde.setPosition(sf::Vector2f(Arena::getOffsetX(), Arena::getOffsetY()));
    borde.setFillColor(sf::Color::Transparent);
    borde.setOutlineColor(sf::Color(230, 230, 230));
    borde.setOutlineThickness(3.f);
    ventana.draw(borde);

    for (const auto& onda : arena.getOndasMelee()) {
        float progreso = 1.f - (onda.tiempoVida / onda.duracionMax);
        float radioActual = onda.radioMax * progreso;

        uint8_t alpha = static_cast<uint8_t>(200 * (1.f - progreso)); // Se va desvaneciendo
        sf::Color colorOnda = onda.esDeLuz ? sf::Color(150, 255, 150, alpha) : sf::Color(255, 150, 150, alpha);

        sf::CircleShape circ(radioActual);
        circ.setFillColor(sf::Color::Transparent);
        circ.setOutlineColor(colorOnda);
        circ.setOutlineThickness(3.f + 4.f * (1.f - progreso));
        circ.setOrigin(sf::Vector2f(radioActual, radioActual));
        circ.setPosition(onda.pos);

        ventana.draw(circ);
    }
    bool tieneNuez = texturas.count("Nuez.png") > 0;
    for (const auto& obs : arena.getObstaculos()) {
        if (tieneNuez) {
            const sf::Texture& tex = texturas.at("Nuez.png");
            sf::Sprite nuez(tex);
            sf::Vector2u texSize = tex.getSize();
            float escala = obs.tam / std::max((float)texSize.x, (float)texSize.y);
            nuez.setScale(sf::Vector2f(escala, escala));
            nuez.setOrigin(sf::Vector2f(texSize.x / 2.f, texSize.y / 2.f));
            nuez.setPosition(obs.pos);
            ventana.draw(nuez);
        }
        else {
            sf::CircleShape c(obs.tam / 2.f);
            c.setFillColor(sf::Color(170, 120, 55));
            c.setOutlineColor(sf::Color(90, 55, 25));
            c.setOutlineThickness(2.f);
            c.setPosition(sf::Vector2f(obs.pos.x - obs.tam / 2.f, obs.pos.y - obs.tam / 2.f));
            ventana.draw(c);
        }
    }

    for (const auto& p : arena.getProyectiles()) {
        if (!p.activo) continue;

        std::string nombreTirador = p.esDeLuz
            ? arena.getCombatiente1().pieza->getNombre()
            : arena.getCombatiente2().pieza->getNombre();
        if (p.esDeLuz && arena.getCombatiente1().pieza->getBando() != LUZ)
            nombreTirador = arena.getCombatiente2().pieza->getNombre();
        else if (!p.esDeLuz && arena.getCombatiente2().pieza->getBando() == LUZ)
            nombreTirador = arena.getCombatiente1().pieza->getNombre();

        bool dibujado = false;
        if (texturasProyectil.count(nombreTirador) > 0) {
            const sf::Texture& tex = texturasProyectil.at(nombreTirador);
            sf::Sprite sp(tex);
            sf::Vector2u ts = tex.getSize();
            float tam = Arena::getTamProyectil() * 2.5f;
            float esc = tam / std::max((float)ts.x, (float)ts.y);
            sp.setScale(sf::Vector2f(esc, esc));
            sp.setOrigin(sf::Vector2f(ts.x / 2.f, ts.y / 2.f));
            sp.setPosition(p.pos);
            if (nombreTirador == "Ingeniero" || nombreTirador == "Dave el Loco") {
                float angulo = relojAnimacion.getElapsedTime().asSeconds() * 360.f;
                sp.setRotation(sf::degrees(angulo));
            }
            ventana.draw(sp);
            dibujado = true;
        }

        if (!dibujado) {
            sf::Color col = p.esDeLuz ? sf::Color(100, 255, 120) : sf::Color(255, 80, 80);
            sf::CircleShape circulo(Arena::getTamProyectil() / 2.f);
            circulo.setFillColor(col);
            circulo.setOutlineColor(sf::Color::White);
            circulo.setOutlineThickness(1.f);
            circulo.setOrigin(sf::Vector2f(Arena::getTamProyectil() / 2.f, Arena::getTamProyectil() / 2.f));
            circulo.setPosition(p.pos);
            ventana.draw(circulo);
        }
    }

    if (arena.getCombatiente1().pieza != nullptr)
        dibujarCombatienteArena(arena.getCombatiente1(), true);

    if (arena.getCombatiente2().pieza != nullptr)
        dibujarCombatienteArena(arena.getCombatiente2(), false);

    if (arena.getCombatiente1().pieza != nullptr &&
        arena.getCombatiente2().pieza != nullptr)
        dibujarHUDArena(arena.getCombatiente1().pieza, arena.getCombatiente2().pieza);
}

void Renderer::dibujarCombatienteArena(const CombatienteArena& c, bool esLuz) {
    float radio = Arena::getTamPieza();
    std::string clave = claveAnimacion(c.pieza->getNombre());
    bool dibujadoConSheet = false;

    if (!clave.empty()) {
        const InfoSheet* sheet = nullptr;
        if (c.estadoAnim == ANIM_ATTACK
            && sheetsAttack.count(clave) > 0
            && sheetsAttack.at(clave).cargada)
            sheet = &sheetsAttack.at(clave);
        else if (sheetsIdle.count(clave) > 0 && sheetsIdle.at(clave).cargada)
            sheet = &sheetsIdle.at(clave);

        if (sheet != nullptr && sheet->cargada && sheet->anchoFrame > 0) {
            sf::Sprite sprite(sheet->textura);
            int frameIdx = c.frameActual % sheet->numFrames;
            int offX = sheet->offsetsX.empty() ? frameIdx * sheet->anchoFrame : sheet->offsetsX[frameIdx];
            int anchof = sheet->anchosFrame.empty() ? sheet->anchoFrame : sheet->anchosFrame[frameIdx];
            sprite.setTextureRect(sf::IntRect(
                sf::Vector2i(offX, 0),
                sf::Vector2i(anchof, sheet->altoFrame)
            ));
            float tamDeseado = radio * 3.5f;
            float escala = tamDeseado / std::max((float)sheet->anchoFrame, (float)sheet->altoFrame);

            sprite.setOrigin(sf::Vector2f(sheet->anchoFrame / 2.f, sheet->altoFrame / 2.f));
            sprite.setPosition(sf::Vector2f(c.pos.x, c.pos.y));

            static const std::vector<std::string> miranaIzq = {
                 "Yeti", "Balloon", "Catapult", "Football", "Digger",
                "Dr. Zomboss"
            };
            bool yaIzq = std::find(miranaIzq.begin(), miranaIzq.end(), clave) != miranaIzq.end();

            bool voltear = (!esLuz && !yaIzq) || (esLuz && yaIzq);

            if (voltear)
                sprite.setScale(sf::Vector2f(-escala, escala));
            else
                sprite.setScale(sf::Vector2f(escala, escala));

            ventana.draw(sprite);
            dibujadoConSheet = true;
        }
    }

    if (!dibujadoConSheet) {
        std::string archivo = nombreArchivoSprite(c.pieza->getNombre());
        bool tieneSprite = !archivo.empty() && texturas.count(archivo) > 0;

        if (tieneSprite) {
            const sf::Texture& tex = texturas.at(archivo);
            sf::Sprite sprite(tex);
            float tam = radio * 2.5f;
            sf::Vector2u texSize = tex.getSize();
            float escala = tam / std::max((float)texSize.x, (float)texSize.y);
            sprite.setScale(sf::Vector2f(escala, escala));
            float spriteW = texSize.x * escala;
            float spriteH = texSize.y * escala;
            sprite.setPosition(sf::Vector2f(c.pos.x - spriteW / 2.f, c.pos.y - spriteH / 2.f));
            ventana.draw(sprite);
        }
        else {
            sf::Color col = esLuz ? sf::Color(80, 200, 110) : sf::Color(200, 70, 70);
            sf::CircleShape circ(radio);
            circ.setFillColor(col);
            circ.setOutlineColor(sf::Color::White);
            circ.setOutlineThickness(2.f);
            circ.setPosition(sf::Vector2f(c.pos.x - radio, c.pos.y - radio));
            ventana.draw(circ);
        }
    }

    float anchoVida = 60.f;
    dibujarBarraVida(c.pieza->vida, c.pieza->vidaMaxima,
        c.pos.x - anchoVida / 2.f, c.pos.y - radio - 14.f, anchoVida);
    dibujarBarraCooldown(c.tiempoRecarga, c.tiempoRecargaMax,
        c.pos.x - anchoVida / 2.f, c.pos.y - radio - 7.f, anchoVida);
}

void Renderer::dibujarHUDArena(Pieza* p1, Pieza* p2) {
    float yHUD = Arena::getOffsetY() + 5.f;
    float anchoHUD = 300.f;

    if (fuenteCargada) {
        sf::Text nom1(fuente, p1->getNombre() + "  WASD+Space", 13);
        nom1.setFillColor(sf::Color(100, 230, 120));
        nom1.setStyle(sf::Text::Bold);
        nom1.setPosition(sf::Vector2f(Arena::getOffsetX() + 5.f, yHUD));
        ventana.draw(nom1);

        sf::Text nomZombie(fuente, p2->getNombre() + "  Flechas+Enter", 13);
        nomZombie.setFillColor(sf::Color(230, 100, 100));
        nomZombie.setStyle(sf::Text::Bold);
        sf::FloatRect b = nomZombie.getLocalBounds();
        nomZombie.setPosition(sf::Vector2f(
            Arena::getOffsetX() + Arena::getAncho() - b.size.x - 5.f,
            yHUD
        ));
        ventana.draw(nomZombie);
    }

    dibujarBarraVida(p1->vida, p1->vidaMaxima, Arena::getOffsetX() + 5.f, yHUD + 20.f, anchoHUD);
    dibujarBarraVida(p2->vida, p2->vidaMaxima,
        Arena::getOffsetX() + Arena::getAncho() - anchoHUD - 5.f, yHUD + 20.f, anchoHUD);
}

void Renderer::seleccionarCasilla(int fila, int col, Tablero* tablero) {
    filaSeleccionada = fila;
    colSeleccionada = col;
    cursorFila = fila;
    cursorCol = col;
    movimientosDisponibles.clear();

    if (tablero == nullptr) return;
    Pieza* p = tablero->getPieza(fila, col);
    if (p == nullptr) return;

    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            if (f == fila && c == col) continue;
            if (tablero->esMovimientoValido(fila, col, f, c))
                movimientosDisponibles.push_back({ f, c });
        }
    }
}

void Renderer::deseleccionar() {
    filaSeleccionada = -1;
    colSeleccionada = -1;
    movimientosDisponibles.clear();
}

bool Renderer::pixelACasilla(int px, int py, int& fila, int& col) {
    int fx = px - (int)OFFSET_X;
    int fy = py - (int)OFFSET_Y;
    if (fx < 0 || fy < 0) return false;
    col = fx / (int)TAM_CASILLA;
    fila = fy / (int)TAM_CASILLA;
    if (fila >= 9 || col >= 9) return false;
    return true;
}

void Renderer::moverCursor(int dFila, int dCol) {
    if (filaSeleccionada != -1 && !movimientosDisponibles.empty() && modoHechizo == SIN_HECHIZO) {
        int mejorFila = cursorFila, mejorCol = cursorCol, mejorDist = 999;
        for (auto& pos : movimientosDisponibles) {
            int f = pos.first, c = pos.second;
            bool enDireccion = false;
            if (dFila < 0 && f < cursorFila) enDireccion = true;
            if (dFila > 0 && f > cursorFila) enDireccion = true;
            if (dCol < 0 && c < cursorCol)  enDireccion = true;
            if (dCol > 0 && c > cursorCol)  enDireccion = true;
            if (!enDireccion) continue;
            int dist = std::abs(f - cursorFila) + std::abs(c - cursorCol);
            if (dist < mejorDist) { mejorDist = dist; mejorFila = f; mejorCol = c; }
        }
        cursorFila = mejorFila;
        cursorCol = mejorCol;
    }
    else {
        int nuevaFila = cursorFila + dFila;
        int nuevaCol = cursorCol + dCol;
        if (nuevaFila < 0 || nuevaFila > 8 || nuevaCol < 0 || nuevaCol > 8) return;
        cursorFila = nuevaFila;
        cursorCol = nuevaCol;
    }
}

sf::Vector2f Renderer::getCentroCasilla(int fila, int col) const {
    return sf::Vector2f(
        OFFSET_X + col * TAM_CASILLA + TAM_CASILLA / 2.f,
        OFFSET_Y + fila * TAM_CASILLA + TAM_CASILLA / 2.f
    );
}

void Renderer::dibujarPiezaPixel(Pieza* p, float px, float py) {
    dibujarPiezaPixelEscala(p, px, py, 1.f);
}

void Renderer::dibujarPiezaPixelEscala(Pieza* p, float px, float py, float escala) {
    if (escala <= 0.f) return;
    std::string archivo = nombreArchivoSprite(p->getNombre());
    bool tieneSprite = !archivo.empty() && texturas.count(archivo) > 0;

    if (tieneSprite) {
        const sf::Texture& tex = texturas.at(archivo);
        sf::Sprite sprite(tex);
        float margen = 6.f;
        float tamDisp = (TAM_CASILLA - margen * 2.f) * escala;
        sf::Vector2u texSize = tex.getSize();
        float esc = tamDisp / std::max((float)texSize.x, (float)texSize.y);
        sprite.setScale(sf::Vector2f(esc, esc));
        float spriteW = texSize.x * esc;
        float spriteH = texSize.y * esc;
        sprite.setPosition(sf::Vector2f(px - spriteW / 2.f, py - spriteH / 2.f));
        ventana.draw(sprite);
    }
    else {
        sf::Color colorPieza = (p->getBando() == LUZ)
            ? sf::Color(80, 180, 100)
            : sf::Color(180, 60, 60);
        float radio = TAM_CASILLA * 0.35f * escala;
        sf::CircleShape circulo(radio);
        circulo.setFillColor(colorPieza);
        circulo.setOutlineColor(sf::Color::Black);
        circulo.setOutlineThickness(2.f);
        circulo.setOrigin(sf::Vector2f(radio, radio));
        circulo.setPosition(sf::Vector2f(px, py));
        ventana.draw(circulo);
    }
}

void Renderer::dibujarPuntosDePoder(Tablero* tablero) {
    const std::pair<int, int> puntos[5] = { {0,4}, {4,0}, {4,4}, {4,8}, {8,4} };
    float t = relojAnimacion.getElapsedTime().asSeconds();
    float pulso = 0.5f + 0.5f * std::sin(t * 1.8f);
    uint8_t alpha = static_cast<uint8_t>(90 + static_cast<int>(100 * pulso));

    for (const auto& [f, c] : puntos) {
        float cx = OFFSET_X + c * TAM_CASILLA + TAM_CASILLA / 2.f;
        float cy = OFFSET_Y + f * TAM_CASILLA + TAM_CASILLA / 2.f;
        float radio = TAM_CASILLA * 0.13f;

        sf::Color colorDiamante(255u, 210u, 0u, alpha);
        sf::Color colorBorde(200u, 160u, 0u, alpha);

        Pieza* p = tablero->getPieza(f, c);
        if (p != nullptr) {
            if (p->getBando() == LUZ) {
                colorDiamante = sf::Color(80u, 220u, 120u, alpha + 60);
                colorBorde = sf::Color(40u, 160u, 80u, alpha + 60);
            }
            else {
                colorDiamante = sf::Color(220u, 80u, 80u, alpha + 60);
                colorBorde = sf::Color(160u, 40u, 40u, alpha + 60);
            }
        }

        sf::CircleShape diamante(radio, 4);
        diamante.setFillColor(colorDiamante);
        diamante.setOutlineColor(colorBorde);
        diamante.setOutlineThickness(1.5f);
        diamante.setRotation(sf::degrees(45.f));
        diamante.setOrigin(sf::Vector2f(radio, radio));
        diamante.setPosition(sf::Vector2f(cx, cy));
        ventana.draw(diamante);
    }
}

void Renderer::dibujarPantallaVictoria(
    Bando ganador,
    TipoVictoria tipoVictoria,
    int bajasLuz,
    int bajasOscuridad,
    int bajasTotales,
    int turnosJugados
) {
    bool esLuz = (ganador == LUZ);
    std::string fondoVictoria = esLuz ? "Plantas_Victoria.png" : "Zombies_Victoria.png";

    if (texturas.count(fondoVictoria) > 0) {
        const sf::Texture& tex = texturas.at(fondoVictoria);
        sf::Sprite fondo(tex);
        sf::Vector2u texSize = tex.getSize();
        float escalaX = VENTANA_ANCHO / (float)texSize.x;
        float escalaY = VENTANA_ALTO / (float)texSize.y;
        fondo.setScale(sf::Vector2f(escalaX, escalaY));
        fondo.setPosition(sf::Vector2f(0.f, 0.f));
        ventana.draw(fondo);
    }
    else {
        sf::RectangleShape fondo(sf::Vector2f(VENTANA_ANCHO, VENTANA_ALTO));
        fondo.setFillColor(esLuz ? sf::Color(10, 45, 20) : sf::Color(45, 10, 20));
        ventana.draw(fondo);
    }

    sf::RectangleShape overlay(sf::Vector2f(VENTANA_ANCHO, VENTANA_ALTO));
    overlay.setFillColor(sf::Color(0u, 0u, 0u, 120u));
    ventana.draw(overlay);

    sf::Color colorGanador = esLuz ? sf::Color(80u, 230u, 120u) : sf::Color(230u, 70u, 70u);
    sf::Color colorPanel = esLuz ? sf::Color(10u, 55u, 25u, 220u) : sf::Color(55u, 10u, 20u, 220u);

    float t = relojAnimacion.getElapsedTime().asSeconds();
    float pulso = 0.5f + 0.5f * std::sin(t * 2.5f);
    uint8_t alphaBorde = static_cast<uint8_t>(170 + static_cast<int>(85 * pulso));
    uint8_t alphaTexto = static_cast<uint8_t>(190 + static_cast<int>(65 * pulso));

    float panelW = 700.f, panelH = 390.f;
    float panelX = (VENTANA_ANCHO - panelW) / 2.f;
    float panelY = (VENTANA_ALTO - panelH) / 2.f + 35.f;

    sf::RectangleShape sombra(sf::Vector2f(panelW + 18.f, panelH + 18.f));
    sombra.setPosition(sf::Vector2f(panelX - 9.f, panelY + 9.f));
    sombra.setFillColor(sf::Color(0u, 0u, 0u, 150u));
    ventana.draw(sombra);

    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition(sf::Vector2f(panelX, panelY));
    panel.setFillColor(colorPanel);
    panel.setOutlineColor(sf::Color(colorGanador.r, colorGanador.g, colorGanador.b, alphaBorde));
    panel.setOutlineThickness(5.f);
    ventana.draw(panel);

    sf::RectangleShape linea(sf::Vector2f(panelW - 70.f, 3.f));
    linea.setPosition(sf::Vector2f(panelX + 35.f, panelY + 82.f));
    linea.setFillColor(sf::Color(colorGanador.r, colorGanador.g, colorGanador.b, 190u));
    ventana.draw(linea);

    if (!fuenteCargada) return;

    auto textoC = [&](const std::string& txt, int tam, sf::Color col, float y) {
        sf::Text t2(fuente, txt, (unsigned)tam);
        t2.setFillColor(col);
        sf::FloatRect b = t2.getLocalBounds();
        t2.setPosition(sf::Vector2f(VENTANA_ANCHO / 2.f - b.size.x / 2.f, y));
        ventana.draw(t2);
        };

    sf::Text tv(fuente, esLuz ? "PLANTAS GANAN!" : "ZOMBIES GANAN!", 54);
    tv.setFillColor(sf::Color(colorGanador.r, colorGanador.g, colorGanador.b, alphaTexto));
    tv.setStyle(sf::Text::Bold);
    sf::FloatRect b1 = tv.getLocalBounds();
    tv.setPosition(sf::Vector2f(VENTANA_ANCHO / 2.f - b1.size.x / 2.f, panelY + 25.f));
    ventana.draw(tv);

    textoC(esLuz ? "El jardin ha sido defendido" : "Los zombies han tomado el jardin",
        24, sf::Color(230u, 230u, 230u), panelY + 103.f);

    std::string txtTipo = (tipoVictoria == VICTORIA_ELIMINACION)
        ? "Victoria por eliminacion total"
        : (tipoVictoria == VICTORIA_PUNTOS_PODER)
        ? "Victoria por control de puntos de poder"
        : "Partida finalizada";
    textoC(txtTipo, 23, sf::Color::White, panelY + 158.f);
    textoC("Turnos jugados: " + std::to_string(turnosJugados), 21, sf::Color(220u, 220u, 220u), panelY + 205.f);
    textoC("Piezas eliminadas: " + std::to_string(bajasTotales), 21, sf::Color(220u, 220u, 220u), panelY + 235.f);
    textoC("Bajas plantas: " + std::to_string(bajasLuz), 18, sf::Color(120u, 240u, 150u), panelY + 265.f);
    textoC("Bajas zombies: " + std::to_string(bajasOscuridad), 18, sf::Color(240u, 120u, 120u), panelY + 293.f);
    textoC("Pulsa ENTER o haz click para volver al menu", 18, sf::Color(190u, 190u, 190u), panelY + 345.f);
}

void Renderer::dibujarPanelHechizos(Bando turno, bool* hechizosUsadosLuz, bool* hechizosUsadosOscuridad, int hechizoSeleccionado) {
    if (!fuenteCargada) return;
    bool* listaUsados = (turno == LUZ) ? hechizosUsadosLuz : hechizosUsadosOscuridad;

    std::vector<std::string> nombres = {
        "1. Curacion", "2. Teleport", "3. Daño",
        "4. Ralentizar", "5. Fortalecer", "6. Escudo", "7. Congelar"
    };

    float yInicio = 820.f;
    float xInicio = OFFSET_X;
    float anchoCasilla = HUD_ANCHO / 7.f;
    float altoCasilla = 50.f;

    for (int i = 0; i < 7; i++) {
        float x = xInicio + i * anchoCasilla;
        float y = yInicio;

        sf::RectangleShape fondo(sf::Vector2f(anchoCasilla - 5.f, altoCasilla));
        fondo.setPosition(sf::Vector2f(x, y));

        if (listaUsados[i])
            fondo.setFillColor(sf::Color(60, 60, 60));
        else if (i == hechizoSeleccionado)
            fondo.setFillColor(sf::Color(215, 215, 0, 180));
        else
            fondo.setFillColor(turno == LUZ ? sf::Color(30, 80, 40) : sf::Color(80, 30, 30));

        fondo.setOutlineColor(sf::Color(200, 200, 200));
        fondo.setOutlineThickness(1.f);
        ventana.draw(fondo);

        sf::Text texto(fuente, nombres[i], 11);
        texto.setFillColor(listaUsados[i] ? sf::Color(120, 120, 120) : sf::Color::White);
        texto.setPosition(sf::Vector2f(x + 4.f, y + 16.f));
        ventana.draw(texto);
    }
}

void Renderer::dibujarMenuPausa() {
    // Overlay oscuro sobre el juego pausado
    sf::RectangleShape overlay(sf::Vector2f(VENTANA_ANCHO, VENTANA_ALTO));
    overlay.setFillColor(sf::Color(0u, 0u, 0u, 170u));
    ventana.draw(overlay);

    // Panel central
    float panelW = 500.f, panelH = 360.f;
    float panelX = (VENTANA_ANCHO - panelW) / 2.f;
    float panelY = (VENTANA_ALTO  - panelH) / 2.f;

    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition(sf::Vector2f(panelX, panelY));
    panel.setFillColor(sf::Color(18u, 18u, 30u, 230u));
    panel.setOutlineColor(sf::Color(120u, 120u, 200u));
    panel.setOutlineThickness(3.f);
    ventana.draw(panel);

    if (!fuenteCargada) return;

    // Titulo
    sf::Text titulo(fuente, "PAUSA", 48);
    titulo.setFillColor(sf::Color(180u, 180u, 255u));
    titulo.setStyle(sf::Text::Bold);
    sf::FloatRect tb = titulo.getLocalBounds();
    titulo.setPosition(sf::Vector2f(VENTANA_ANCHO / 2.f - tb.size.x / 2.f, panelY + 20.f));
    ventana.draw(titulo);

    sf::RectangleShape sep(sf::Vector2f(panelW - 60.f, 2.f));
    sep.setPosition(sf::Vector2f(panelX + 30.f, panelY + 80.f));
    sep.setFillColor(sf::Color(100u, 100u, 180u, 160u));
    ventana.draw(sep);

    // Opciones con teclas
    struct Opcion { std::string tecla; std::string texto; };
    Opcion opciones[] = {
        { "P / ESC", "Reanudar partida" },
        { "R",       "Reiniciar partida" },
        { "M",       "Volver al menu principal" }
    };

    float yBase = panelY + 100.f;
    for (const auto& op : opciones) {
        sf::Text tTecla(fuente, "[" + op.tecla + "]", 22);
        tTecla.setFillColor(sf::Color(140u, 200u, 255u));
        tTecla.setStyle(sf::Text::Bold);
        tTecla.setPosition(sf::Vector2f(panelX + 35.f, yBase));
        ventana.draw(tTecla);

        sf::Text tOpc(fuente, op.texto, 22);
        tOpc.setFillColor(sf::Color(220u, 220u, 220u));
        tOpc.setPosition(sf::Vector2f(panelX + 200.f, yBase));
        ventana.draw(tOpc);

        yBase += 70.f;
    }
}