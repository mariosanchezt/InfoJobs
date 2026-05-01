#include "Renderer.h"
#include <iostream>
#include <cstdlib>

Renderer::Renderer(sf::RenderWindow& vent)
    : ventana(vent)
{
    fuenteCargada = false;
    texturasCargadas = false;
    estado = TABLERO;

    filaSeleccionada = -1;
    colSeleccionada = -1;

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

// Mapea el nombre de la pieza al archivo PNG correspondiente
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
    if (nombrePieza == "Dave el Loco")      return "Dave el Loco.png";
    if (nombrePieza == "Dr. Zomboss")      return "Dr. Zomboss.png";
    return "";
}

void Renderer::cargarSprites(const std::string& carpeta) {
    std::vector<std::string> archivos = {
        "Chomper.png", "Peashooter.png", "Sunflower.png", "Citron.png", "Kernelpult.png",
        "SuperBrainz.png", "Soldier.png", "Imp.png", "AllStar.png", "Engineer.png", 
        "Dave el Loco.png", "Dr. Zomboss.png"

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
            std::cout << "Aviso: no se encontro " << ruta << " - usando circulo" << std::endl;
        }
    }
    texturasCargadas = (cargados > 0);
    std::cout << cargados << "/12 sprites cargados." << std::endl;
}


// TABLERO

void Renderer::dibujarEstadoTablero(Tablero* tablero, Bando turno) {
    if (tablero == nullptr) return;
    dibujarTablero(tablero);
    dibujarMovimientosDisponibles();
    if (filaSeleccionada >= 0 && colSeleccionada >= 0)
        dibujarCasillaResaltada(filaSeleccionada, colSeleccionada);
    dibujarIndicadorTurno(turno);
}

void Renderer::dibujarTablero(Tablero* tablero) {
    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            int colorActual = tablero->getColorActual(f, c);
            sf::Color colorFondo;
            if (colorActual == 0) colorFondo = colorBlanco;
            else if (colorActual == 1) colorFondo = colorNegro;
            else                       colorFondo = colorGris;

            sf::RectangleShape casilla(sf::Vector2f(TAM_CASILLA, TAM_CASILLA));
            casilla.setPosition(sf::Vector2f(OFFSET_X + c * TAM_CASILLA, OFFSET_Y + f * TAM_CASILLA));
            casilla.setFillColor(colorFondo);
            casilla.setOutlineColor(sf::Color(50u, 50u, 50u));
            casilla.setOutlineThickness(1.f);
            ventana.draw(casilla);

            Pieza* p = tablero->getPieza(f, c);
            if (p != nullptr) dibujarPieza(p, f, c);
        }
    }
}

void Renderer::dibujarPieza(Pieza* p, int fila, int col) {
    float px = OFFSET_X + col * TAM_CASILLA;
    float py = OFFSET_Y + fila * TAM_CASILLA;

    std::string archivo = nombreArchivoSprite(p->getNombre());
    bool tieneSprite = !archivo.empty() && texturas.count(archivo) > 0;

    if (tieneSprite) {
        // Sprite escalado con margen dentro de la casilla
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
        // Fallback: circulo de color si no hay sprite
        sf::Color colorPieza = (p->getBando() == LUZ)
            ? sf::Color(80, 180, 100)
            : sf::Color(180, 60, 60);

        float radio = TAM_CASILLA * 0.35f;
        sf::CircleShape circulo(radio);
        circulo.setFillColor(colorPieza);
        circulo.setOutlineColor(sf::Color::Black);
        circulo.setOutlineThickness(2.f);
        circulo.setPosition(sf::Vector2f(
            px + TAM_CASILLA / 2.f - radio,
            py + TAM_CASILLA / 2.f - radio - 8.f
        ));
        ventana.draw(circulo);
    }

    // Barra de vida siempre encima
    dibujarBarraVida(p->vida, p->vidaMaxima, px + 5.f, py + 4.f, TAM_CASILLA - 10.f);

    
}

void Renderer::dibujarCasillaResaltada(int fila, int col) {
    sf::RectangleShape resalte(sf::Vector2f(TAM_CASILLA, TAM_CASILLA));
    resalte.setPosition(sf::Vector2f(OFFSET_X + col * TAM_CASILLA, OFFSET_Y + fila * TAM_CASILLA));
    resalte.setFillColor(colorSeleccion);
    resalte.setOutlineColor(sf::Color(255u, 200u, 0u));
    resalte.setOutlineThickness(3.f);
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
    else                        colorVida = sf::Color(220, 50, 50);

    sf::RectangleShape relleno(sf::Vector2f(ancho * porcentaje, 5.f));
    relleno.setPosition(sf::Vector2f(x, y));
    relleno.setFillColor(colorVida);
    ventana.draw(relleno);
}

void Renderer::dibujarIndicadorTurno(Bando turno) {
    float yFranja = OFFSET_Y + 9.f * TAM_CASILLA + 5.f;
    bool esLuz = (turno == LUZ);

    sf::Color colorFranja = esLuz ? sf::Color(50, 130, 70) : sf::Color(130, 40, 40);
    sf::RectangleShape franja(sf::Vector2f(760.f, 50.f));
    franja.setPosition(sf::Vector2f(OFFSET_X, yFranja));
    franja.setFillColor(colorFranja);
    ventana.draw(franja);

    if (fuenteCargada) {
        std::string textoTurno = esLuz ? "TURNO: PLANTAS" : "TURNO: ZOMBIES";
        sf::Text texto(fuente, textoTurno, 20);
        texto.setFillColor(sf::Color::White);
        texto.setStyle(sf::Text::Bold);
        sf::FloatRect b = texto.getLocalBounds();
        texto.setPosition(sf::Vector2f(400.f - b.size.x / 2.f, yFranja + 13.f));
        ventana.draw(texto);
    }
}

// Arena basica (sin interactividad aun)

void Renderer::dibujarEstadoArena(Pieza* p1, Pieza* p2) {
    if (p1 == nullptr || p2 == nullptr) return;

    ventana.clear(sf::Color(30, 20, 40));

    if (fuenteCargada) {
        sf::Text titulo(fuente, "--- COMBATE ---", 28);
        titulo.setFillColor(sf::Color(255, 220, 80));
        titulo.setStyle(sf::Text::Bold);
        sf::FloatRect b = titulo.getLocalBounds();
        titulo.setPosition(sf::Vector2f(400.f - b.size.x / 2.f, 30.f));
        ventana.draw(titulo);

        // Nombre y vida p1
        sf::Text nom1(fuente, p1->getNombre(), 22);
        nom1.setFillColor(sf::Color(100, 230, 120));
        nom1.setStyle(sf::Text::Bold);
        sf::FloatRect b1 = nom1.getLocalBounds();
        nom1.setPosition(sf::Vector2f(200.f - b1.size.x / 2.f, 120.f));
        ventana.draw(nom1);
        dibujarBarraVida(p1->vida, p1->vidaMaxima, 80.f, 160.f, 240.f);

        // VS
        sf::Text vs(fuente, "VS", 36);
        vs.setFillColor(sf::Color(255, 100, 100));
        vs.setStyle(sf::Text::Bold);
        sf::FloatRect bv = vs.getLocalBounds();
        vs.setPosition(sf::Vector2f(400.f - bv.size.x / 2.f, 130.f));
        ventana.draw(vs);

        // Nombre y vida p2
        sf::Text nom2(fuente, p2->getNombre(), 22);
        nom2.setFillColor(sf::Color(230, 100, 100));
        nom2.setStyle(sf::Text::Bold);
        sf::FloatRect b2 = nom2.getLocalBounds();
        nom2.setPosition(sf::Vector2f(600.f - b2.size.x / 2.f, 120.f));
        ventana.draw(nom2);
        dibujarBarraVida(p2->vida, p2->vidaMaxima, 480.f, 160.f, 240.f);
    }
}


// Seleccion de casillas

void Renderer::seleccionarCasilla(int fila, int col, Tablero* tablero) {
    filaSeleccionada = fila;
    colSeleccionada = col;
    movimientosDisponibles.clear();

    if (tablero == nullptr) return;
    Pieza* p = tablero->getPieza(fila, col);
    if (p == nullptr) return;

    int radio = p->radioMovimiento;
    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            if (f == fila && c == col) continue;
            if (tablero->esMovimientoValido(fila, col, f, c)) {
                Pieza* ocupante = tablero->getPieza(f, c);
                if (ocupante == nullptr || ocupante->getBando() != p->getBando()) {
                    movimientosDisponibles.push_back({ f,c });
                }
            }
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