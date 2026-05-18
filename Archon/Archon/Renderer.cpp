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
    if (nombrePieza == "Girasol")        return "Sunflower.png";
    if (nombrePieza == "Pomelo")         return "Citron.png";
    if (nombrePieza == "Mazorca")        return "Kernelpult.png";
    if (nombrePieza == "Supercerebroz")  return "SuperBrainz.png";
    if (nombrePieza == "Soldado")        return "Soldier.png";
    if (nombrePieza == "Zombidito")      return "Imp.png";
    if (nombrePieza == "All-Star")       return "AllStar.png";
    if (nombrePieza == "Ingeniero")      return "Engineer.png";
    if (nombrePieza == "Dave el Loco")   return "Dave el Loco.png";
    if (nombrePieza == "Dr. Zomboss")    return "Dr. Zomboss.png";
    return "";
}

void Renderer::cargarSprites(const std::string& carpeta) {
    std::vector<std::string> archivos = {
        "Chomper.png", "Peashooter.png", "Sunflower.png", "Citron.png", "Kernelpult.png",
        "SuperBrainz.png", "Soldier.png", "Imp.png", "AllStar.png", "Engineer.png",
        "Dave el Loco.png", "Dr. Zomboss.png",

        // Recursos visuales de la arena
        "patio.png",
        "Nuez.png"
    };

    int cargados = 0;

    for (const auto& archivo : archivos) {
        std::string ruta = carpeta + "/" + archivo;

        sf::Texture tex;

        if (tex.loadFromFile(ruta)) {
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
            if (animTeleport.fase == FASE_HUNDIR)  animTeleport.fase = FASE_MOVER;
            else if (animTeleport.fase == FASE_MOVER)   animTeleport.fase = FASE_EMERGER;
            else if (animTeleport.fase == FASE_EMERGER) animTeleport.fase = FASE_NINGUNA;
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
    borde.setPosition(sf::Vector2f(OFFSET_X + col * TAM_CASILLA + 2.f, OFFSET_Y + fila * TAM_CASILLA + 2.f));
    borde.setFillColor(sf::Color::Transparent);
    borde.setOutlineColor(color);
    borde.setOutlineThickness(3.f);
    ventana.draw(borde);
}

void Renderer::dibujarAnimaciones() {
    for (const auto& anim : animaciones) {
        float progreso = 1.f - (anim.tiempoRestante / anim.tiempoTotal);
        float cx = OFFSET_X + anim.col * TAM_CASILLA + TAM_CASILLA / 2.f;
        float cy = OFFSET_Y + anim.fila * TAM_CASILLA + TAM_CASILLA / 2.f;

        uint8_t alpha = static_cast<uint8_t>(220 * (1.f - progreso));
        float radio = TAM_CASILLA * 0.2f + TAM_CASILLA * 0.35f * progreso;

        sf::Color colorFlash;

        switch (anim.tipo) {
        case ANIM_CURACION:
            colorFlash = sf::Color(50, 220, 80, alpha);
            break;
        case ANIM_DANO:
            colorFlash = sf::Color(220, 50, 50, alpha);
            break;
        case ANIM_RALENTIZAR:
            colorFlash = sf::Color(220, 180, 30, alpha);
            break;
        case ANIM_FORTALECER:
            colorFlash = sf::Color(255, 200, 0, alpha);
            break;
        case ANIM_ESCUDO:
            colorFlash = sf::Color(80, 160, 255, alpha);
            break;
        case ANIM_CONGELAR:
            colorFlash = sf::Color(120, 200, 255, alpha);
            break;
        default:
            continue;
        }

        sf::CircleShape circulo(radio);
        circulo.setFillColor(sf::Color::Transparent);
        circulo.setOutlineColor(colorFlash);
        circulo.setOutlineThickness(3.f);
        circulo.setOrigin(sf::Vector2f(radio, radio));
        circulo.setPosition(sf::Vector2f(cx, cy));
        ventana.draw(circulo);

        float radio2 = radio * 0.6f;

        sf::CircleShape circulo2(radio2);
        circulo2.setFillColor(sf::Color(colorFlash.r, colorFlash.g, colorFlash.b, alpha / 2));
        circulo2.setOrigin(sf::Vector2f(radio2, radio2));
        circulo2.setPosition(sf::Vector2f(cx, cy));
        ventana.draw(circulo2);
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
        // Fase intermedia: la pieza esta bajo tierra, no dibujamos nada
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
        dibujarBordeEfecto(piezaCongelada->filaInicial, piezaCongelada->colInicial, sf::Color(120, 200, 255), t);

    if (piezaRalentizada != nullptr)
        dibujarBordeEfecto(piezaRalentizada->filaInicial, piezaRalentizada->colInicial, sf::Color(220, 180, 30), t);

    if (piezaFortalecida != nullptr)
        dibujarBordeEfecto(piezaFortalecida->filaInicial, piezaFortalecida->colInicial, sf::Color(255, 200, 0), t);

    dibujarAnimaciones();
    dibujarTeleportEnCurso();
    dibujarCursor(tablero);

    Pieza* piezaSeleccionada = nullptr;

    if (filaSeleccionada >= 0 && colSeleccionada >= 0)
        piezaSeleccionada = tablero->getPieza(filaSeleccionada, colSeleccionada);

    dibujarIndicadorTurno(turno, piezaSeleccionada, tablero, hechizoSeleccionado);

    if (panelHechizosVisible)
        dibujarPanelHechizos(turno, hechizosUsadosLuz, hechizosUsadosOscuridad, hechizoSeleccionado);

    dibujarCementerio(cementerioLuz, 10.f, 20.f, sf::Color(80, 220, 120), "CEMENTERIO PLANTAS");
    dibujarCementerio(cementerioOscuridad, 870.f, 20.f, sf::Color(220, 80, 80), "CEMENTERIO ZOMBIES");
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

    std::string archivo = nombreArchivoSprite(p->getNombre());
    bool tieneSprite = !archivo.empty() && texturas.count(archivo) > 0;

    if (tieneSprite) {
        const sf::Texture& tex = texturas.at(archivo);
        sf::Sprite sprite(tex);

        float margen = 6.f;
        float tamDisp = TAM_CASILLA - margen * 2.f;

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

    dibujarBarraVida(p->vida, p->vidaMaxima, px + 5.f, py + 4.f, TAM_CASILLA - 10.f);
}

void Renderer::dibujarCasillaResaltada(int fila, int col) {
    float t = relojAnimacion.getElapsedTime().asSeconds();
    float pulso = 0.5f + 0.5f * std::sin(t * 4.f);

    uint8_t alpha = static_cast<uint8_t>(180 + static_cast<int>(75 * pulso));

    sf::Color colorResalte(255u, 215u, 0u, alpha);

    if (filaSeleccionada >= 0 && colSeleccionada >= 0) {
        colorResalte = sf::Color(60u, 255u, 60u, alpha);

        if (col >= 7)
            colorResalte = sf::Color(255u, 60u, 60u, alpha);
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
    fondo.setFillColor(sf::Color(60, 60, 60));
    ventana.draw(fondo);

    sf::Color colorVida;

    if (porcentaje > 0.6f) colorVida = sf::Color(50, 200, 50);
    else if (porcentaje > 0.3f) colorVida = sf::Color(230, 180, 30);
    else colorVida = sf::Color(220, 50, 50);

    sf::RectangleShape relleno(sf::Vector2f(ancho * porcentaje, 5.f));
    relleno.setPosition(sf::Vector2f(x, y));
    relleno.setFillColor(colorVida);
    ventana.draw(relleno);
}

void Renderer::dibujarIndicadorTurno(Bando turno, Pieza* piezaSeleccionada, Tablero* tablero, int hechizoSeleccionado) {
    float yFranja = OFFSET_Y + 9.f * TAM_CASILLA + 5.f;
    bool esLuz = (turno == LUZ);

    sf::Color colorFranja = esLuz ? sf::Color(50, 130, 70) : sf::Color(130, 40, 40);

    if (modoHechizo == HECHIZO_ALIADO) colorFranja = sf::Color(30, 80, 160);
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
    texto.setPosition(sf::Vector2f(OFFSET_X + 380.f - b.size.x / 2.f, yFranja + 15.f));

    ventana.draw(texto);
}

std::string Renderer::tipoMovimientoTexto(TipoMovimiento mov) const {
    switch (mov) {
    case GROUND:
        return "GROUND";
    case FLYING:
        return "FLYING";
    case TELEPORT:
        return "TELEPORT";
    default:
        return "UNKNOWN";
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

void Renderer::dibujarCementerio(
    const std::map<std::string, int>& cementerio,
    float x, float y,
    sf::Color colorTitulo,
    const std::string& titulo
) {
    sf::RectangleShape panel(sf::Vector2f(120.f, 300.f));
    panel.setPosition(sf::Vector2f(x, y));
    panel.setFillColor(sf::Color(20, 20, 20, 190));
    panel.setOutlineColor(colorTitulo);
    panel.setOutlineThickness(2.f);
    ventana.draw(panel);

    if (!fuenteCargada) return;

    sf::Text tituloTxt(fuente, titulo, 11);
    tituloTxt.setFillColor(colorTitulo);
    tituloTxt.setStyle(sf::Text::Bold);
    tituloTxt.setPosition(sf::Vector2f(x + 6.f, y + 8.f));
    ventana.draw(tituloTxt);

    sf::Text icono(fuente, "X", 20);
    icono.setFillColor(sf::Color(180, 180, 180));
    icono.setStyle(sf::Text::Bold);
    icono.setPosition(sf::Vector2f(x + 52.f, y + 30.f));
    ventana.draw(icono);

    float yTexto = y + 65.f;

    if (cementerio.empty()) {
        sf::Text vacio(fuente, "Sin bajas", 13);
        vacio.setFillColor(sf::Color(180, 180, 180));
        vacio.setPosition(sf::Vector2f(x + 10.f, yTexto));
        ventana.draw(vacio);
        return;
    }

    for (const auto& par : cementerio) {
        std::string linea = par.first + " x" + std::to_string(par.second);

        sf::Text texto(fuente, linea, 11);
        texto.setFillColor(sf::Color::White);
        texto.setPosition(sf::Vector2f(x + 8.f, yTexto));
        ventana.draw(texto);

        yTexto += 20.f;
    }
}

// ARENA INTERACTIVA

void Renderer::dibujarEstadoArena(const Arena& arena) {
    // Fondo de arena tipo Plants vs Zombies
    bool tienePatio = texturas.count("patio.png") > 0;

    if (tienePatio) {
        const sf::Texture& tex = texturas.at("patio.png");

        sf::Sprite fondo(tex);

        sf::Vector2u texSize = tex.getSize();

        float escalaX = Arena::getAncho() / (float)texSize.x;
        float escalaY = Arena::getAlto() / (float)texSize.y;

        fondo.setScale(sf::Vector2f(escalaX, escalaY));
        fondo.setPosition(sf::Vector2f(Arena::getOffsetX(), Arena::getOffsetY()));

        ventana.draw(fondo);
    }
    else {
        sf::RectangleShape fondo(sf::Vector2f(Arena::getAncho(), Arena::getAlto()));
        fondo.setPosition(sf::Vector2f(Arena::getOffsetX(), Arena::getOffsetY()));
        fondo.setFillColor(sf::Color(50, 120, 55));
        ventana.draw(fondo);
    }

    // Borde de la arena
    sf::RectangleShape borde(sf::Vector2f(Arena::getAncho(), Arena::getAlto()));
    borde.setPosition(sf::Vector2f(Arena::getOffsetX(), Arena::getOffsetY()));
    borde.setFillColor(sf::Color::Transparent);
    borde.setOutlineColor(sf::Color(230, 230, 230));
    borde.setOutlineThickness(3.f);
    ventana.draw(borde);

    // Obstáculos: nueces
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

    // Proyectiles
    for (const auto& p : arena.getProyectiles()) {
        if (!p.activo) continue;

        sf::Color colProyectil = p.esDeLuz
            ? sf::Color(100, 255, 120)
            : sf::Color(255, 80, 80);

        sf::CircleShape circulo(Arena::getTamProyectil() / 2.f);
        circulo.setFillColor(colProyectil);
        circulo.setOutlineColor(sf::Color::White);
        circulo.setOutlineThickness(1.f);

        circulo.setPosition(sf::Vector2f(
            p.pos.x - Arena::getTamProyectil() / 2.f,
            p.pos.y - Arena::getTamProyectil() / 2.f
        ));

        ventana.draw(circulo);
    }

    dibujarCombatienteArena(
        arena.getCombatiente1(),
        arena.getCombatiente1().pieza->getBando() == LUZ
    );

    dibujarCombatienteArena(
        arena.getCombatiente2(),
        arena.getCombatiente2().pieza->getBando() == LUZ
    );

    dibujarHUDArena(
        arena.getCombatiente1().pieza,
        arena.getCombatiente2().pieza
    );
}

void Renderer::dibujarCombatienteArena(const CombatienteArena& c, bool esLuz) {
    float radio = Arena::getTamPieza();

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

    float anchoVida = 60.f;

    dibujarBarraVida(
        c.pieza->vida,
        c.pieza->vidaMaxima,
        c.pos.x - anchoVida / 2.f,
        c.pos.y - radio - 14.f,
        anchoVida
    );
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

        sf::Text nom2(fuente, p2->getNombre() + "  Flechas+Enter", 13);
        nom2.setFillColor(sf::Color(230, 100, 100));
        nom2.setStyle(sf::Text::Bold);

        sf::FloatRect b = nom2.getLocalBounds();
        nom2.setPosition(sf::Vector2f(Arena::getOffsetX() + Arena::getAncho() - b.size.x - 5.f, yHUD));

        ventana.draw(nom2);
    }

    dibujarBarraVida(
        p1->vida,
        p1->vidaMaxima,
        Arena::getOffsetX() + 5.f,
        yHUD + 20.f,
        anchoHUD
    );

    dibujarBarraVida(
        p2->vida,
        p2->vidaMaxima,
        Arena::getOffsetX() + Arena::getAncho() - anchoHUD - 5.f,
        yHUD + 20.f,
        anchoHUD
    );
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
        int mejorFila = cursorFila;
        int mejorCol = cursorCol;
        int mejorDist = 999;

        for (auto& pos : movimientosDisponibles) {
            int f = pos.first;
            int c = pos.second;

            bool enDireccion = false;

            if (dFila < 0 && f < cursorFila) enDireccion = true;
            if (dFila > 0 && f > cursorFila) enDireccion = true;
            if (dCol < 0 && c < cursorCol) enDireccion = true;
            if (dCol > 0 && c > cursorCol) enDireccion = true;

            if (!enDireccion) continue;

            int dist = std::abs(f - cursorFila) + std::abs(c - cursorCol);

            if (dist < mejorDist) {
                mejorDist = dist;
                mejorFila = f;
                mejorCol = c;
            }
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
    const std::pair<int, int> puntos[5] = {
        {0,4},
        {4,0},
        {4,4},
        {4,8},
        {8,4}
    };

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

void Renderer::dibujarPantallaVictoria(Bando ganador) {
    sf::RectangleShape overlay(sf::Vector2f(1000.f, 900.f));
    overlay.setFillColor(sf::Color(0u, 0u, 0u, 210u));
    ventana.draw(overlay);

    bool esLuz = (ganador == LUZ);

    sf::Color colorGanador = esLuz
        ? sf::Color(80u, 220u, 120u)
        : sf::Color(220u, 80u, 80u);

    float t = relojAnimacion.getElapsedTime().asSeconds();
    float pulso = 0.5f + 0.5f * std::sin(t * 2.5f);

    uint8_t alphaTexto = static_cast<uint8_t>(180 + static_cast<int>(75 * pulso));

    float panelW = 580.f;
    float panelH = 280.f;
    float panelX = (1000.f - panelW) / 2.f;
    float panelY = (900.f - panelH) / 2.f;

    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition(sf::Vector2f(panelX, panelY));
    panel.setFillColor(esLuz ? sf::Color(15u, 50u, 25u) : sf::Color(50u, 15u, 15u));
    panel.setOutlineColor(colorGanador);
    panel.setOutlineThickness(4.f);

    ventana.draw(panel);

    if (!fuenteCargada) return;

    std::string txtVictoria = esLuz ? "PLANTAS GANAN!" : "ZOMBIES GANAN!";

    sf::Text textoVictoria(fuente, txtVictoria, 50);
    textoVictoria.setFillColor(sf::Color(colorGanador.r, colorGanador.g, colorGanador.b, alphaTexto));
    textoVictoria.setStyle(sf::Text::Bold);

    sf::FloatRect b1 = textoVictoria.getLocalBounds();
    textoVictoria.setPosition(sf::Vector2f(500.f - b1.size.x / 2.f, panelY + 60.f));

    ventana.draw(textoVictoria);

    std::string txtSub = esLuz
        ? "Las plantas han tomado el control"
        : "Los zombies han tomado el control";

    sf::Text textoSub(fuente, txtSub, 22);
    textoSub.setFillColor(sf::Color(200u, 200u, 200u));

    sf::FloatRect b2 = textoSub.getLocalBounds();
    textoSub.setPosition(sf::Vector2f(500.f - b2.size.x / 2.f, panelY + 140.f));

    ventana.draw(textoSub);

    sf::Text textoVolver(fuente, "Pulsa ENTER o haz click para volver al menu", 18);
    textoVolver.setFillColor(sf::Color(160u, 160u, 160u));

    sf::FloatRect b3 = textoVolver.getLocalBounds();
    textoVolver.setPosition(sf::Vector2f(500.f - b3.size.x / 2.f, panelY + 210.f));

    ventana.draw(textoVolver);
}

void Renderer::dibujarPanelHechizos(Bando turno, bool* hechizosUsadosLuz, bool* hechizosUsadosOscuridad, int hechizoSeleccionado) {
    if (!fuenteCargada) return;

    bool* listaUsados = (turno == LUZ)
        ? hechizosUsadosLuz
        : hechizosUsadosOscuridad;

    std::vector<std::string> nombres = {
        "1. Curacion",
        "2. Teleport",
        "3. Daño",
        "4. Ralentizar",
        "5. Fortalecer",
        "6. Escudo",
        "7. Congelar"
    };

    float yInicio = 820.f;
    float xInicio = 140.f;
    float anchoCasilla = 105.f;
    float altoCasilla = 50.f;

    for (int i = 0; i < 7; i++) {
        float x = xInicio + i * anchoCasilla;
        float y = yInicio;

        sf::RectangleShape fondo(sf::Vector2f(anchoCasilla - 5.f, altoCasilla));
        fondo.setPosition(sf::Vector2f(x, y));

        if (listaUsados[i]) {
            fondo.setFillColor(sf::Color(60, 60, 60));
        }
        else if (i == hechizoSeleccionado) {
            fondo.setFillColor(sf::Color(215, 215, 0, 180));
        }
        else {
            fondo.setFillColor(turno == LUZ ? sf::Color(30, 80, 40) : sf::Color(80, 30, 30));
        }

        fondo.setOutlineColor(sf::Color(200, 200, 200));
        fondo.setOutlineThickness(1.f);
        ventana.draw(fondo);

        sf::Text texto(fuente, nombres[i], 11);
        texto.setFillColor(listaUsados[i] ? sf::Color(120, 120, 120) : sf::Color::White);
        texto.setPosition(sf::Vector2f(x + 4.f, y + 16.f));

        ventana.draw(texto);
    }
}