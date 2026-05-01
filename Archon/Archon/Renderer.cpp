#include "Renderer.h"
#include <iostream>
#include <cstdlib>

Renderer::Renderer(sf::RenderWindow& vent)
    : ventana(vent)
{
    fuenteCargada = false;
    estado = TABLERO;

    filaSeleccionada = -1;
    colSeleccionada = -1;

    colorBlanco = sf::Color(240, 217, 181);
    colorNegro = sf::Color(90, 120, 80);
    colorGris = sf::Color(160, 160, 140);
    colorSeleccion = sf::Color(255, 215, 0, 180); // amarillo semitransparente
    colorMovimiento = sf::Color(80, 200, 80, 140); // verde semitransparente
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

// Dibuja el estado del tablero (modo normal)
void Renderer::dibujarEstadoTablero(Tablero* tablero, Bando turno) {
    if (tablero == nullptr) return;

    dibujarTablero(tablero);
    dibujarMovimientosDisponibles();

    if (filaSeleccionada >= 0 && colSeleccionada >= 0) {
        dibujarCasillaResaltada(filaSeleccionada, colSeleccionada);
    }

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
            if (p != nullptr) {
                dibujarPieza(p, f, c);
            }
        }
    }
}

void Renderer::dibujarPieza(Pieza* p, int fila, int col) {
    float px = OFFSET_X + col * TAM_CASILLA;
    float py = OFFSET_Y + fila * TAM_CASILLA;

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

    if (fuenteCargada) {
        std::string nombreCorto = p->getNombre().substr(0, 4);

        sf::Text texto(fuente, nombreCorto, 11);
        texto.setFillColor(sf::Color::White);
        texto.setStyle(sf::Text::Bold);

        sf::FloatRect bounds = texto.getLocalBounds();
        texto.setPosition(sf::Vector2f(
            px + TAM_CASILLA / 2.f - bounds.size.x / 2.f,
            py + TAM_CASILLA - 18.f
        ));
        ventana.draw(texto);

        dibujarBarraVida(p->vida, p->vidaMaxima, px + 5.f, py + 4.f, TAM_CASILLA - 10.f);
    }
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

        // Circulo pequeño en el centro pa q se vea bien como en chess.com
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

    sf::Color colorFranja = esLuz
        ? sf::Color(50, 130, 70)
        : sf::Color(130, 40, 40);

    sf::RectangleShape franja(sf::Vector2f(760.f, 50.f));
    franja.setPosition(sf::Vector2f(OFFSET_X, yFranja));
    franja.setFillColor(colorFranja);
    ventana.draw(franja);

    if (fuenteCargada) {
        std::string textoTurno = esLuz ? "TURNO: LUZ (Plantas)" : "TURNO: OSCURIDAD (Zombies)";

        sf::Text texto(fuente, textoTurno, 20);
        texto.setFillColor(sf::Color::White);
        texto.setStyle(sf::Text::Bold);

        sf::FloatRect b = texto.getLocalBounds();
        texto.setPosition(sf::Vector2f(400.f - b.size.x / 2.f, yFranja + 13.f));
        ventana.draw(texto);
    }
}

// Dibuja la pantalla de arena (modo combate)
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
    }

    // PIEZA 1 (izquierda)
    {
        float cx = 180.f;
        float cy = 200.f;

        sf::Color colorP1 = (p1->getBando() == LUZ)
            ? sf::Color(80, 180, 100) : sf::Color(180, 60, 60);

        sf::CircleShape circ(70.f);
        circ.setFillColor(colorP1);
        circ.setOutlineColor(sf::Color::White);
        circ.setOutlineThickness(3.f);
        circ.setPosition(sf::Vector2f(cx - 70.f, cy - 70.f));
        ventana.draw(circ);

        if (fuenteCargada) {
            sf::Text nombre(fuente, p1->getNombre(), 18);
            nombre.setFillColor(sf::Color::White);
            nombre.setStyle(sf::Text::Bold);
            sf::FloatRect b = nombre.getLocalBounds();
            nombre.setPosition(sf::Vector2f(cx - b.size.x / 2.f, cy + 85.f));
            ventana.draw(nombre);

            dibujarBarraVida(p1->vida, p1->vidaMaxima, cx - 80.f, cy + 115.f, 160.f);

            sf::Text vidaTxt(fuente, std::to_string((int)p1->vida) + " / " + std::to_string((int)p1->vidaMaxima), 14);
            vidaTxt.setFillColor(sf::Color(200, 200, 200));
            sf::FloatRect bv = vidaTxt.getLocalBounds();
            vidaTxt.setPosition(sf::Vector2f(cx - bv.size.x / 2.f, cy + 125.f));
            ventana.draw(vidaTxt);
        }
    }

    if (fuenteCargada) {
        sf::Text vs(fuente, "VS", 36);
        vs.setFillColor(sf::Color(255, 100, 100));
        vs.setStyle(sf::Text::Bold);
        sf::FloatRect b = vs.getLocalBounds();
        vs.setPosition(sf::Vector2f(400.f - b.size.x / 2.f, 190.f));
        ventana.draw(vs);
    }

    // PIEZA 2 (derecha)
    {
        float cx = 620.f;
        float cy = 200.f;

        sf::Color colorP2 = (p2->getBando() == LUZ)
            ? sf::Color(80, 180, 100) : sf::Color(180, 60, 60);

        sf::CircleShape circ(70.f);
        circ.setFillColor(colorP2);
        circ.setOutlineColor(sf::Color::White);
        circ.setOutlineThickness(3.f);
        circ.setPosition(sf::Vector2f(cx - 70.f, cy - 70.f));
        ventana.draw(circ);

        if (fuenteCargada) {
            sf::Text nombre(fuente, p2->getNombre(), 18);
            nombre.setFillColor(sf::Color::White);
            nombre.setStyle(sf::Text::Bold);
            sf::FloatRect b = nombre.getLocalBounds();
            nombre.setPosition(sf::Vector2f(cx - b.size.x / 2.f, cy + 85.f));
            ventana.draw(nombre);

            dibujarBarraVida(p2->vida, p2->vidaMaxima, cx - 80.f, cy + 115.f, 160.f);

            sf::Text vidaTxt(fuente, std::to_string((int)p2->vida) + " / " + std::to_string((int)p2->vidaMaxima), 14);
            vidaTxt.setFillColor(sf::Color(200, 200, 200));
            sf::FloatRect bv = vidaTxt.getLocalBounds();
            vidaTxt.setPosition(sf::Vector2f(cx - bv.size.x / 2.f, cy + 125.f));
            ventana.draw(vidaTxt);
        }
    }
}

// Seleccion de casillas y calculo de movimientos
void Renderer::seleccionarCasilla(int fila, int col, Tablero* tablero) {
    filaSeleccionada = fila;
    colSeleccionada = col;
    movimientosDisponibles.clear();

    if (tablero == nullptr) return;

    Pieza* p = tablero->getPieza(fila, col);
    if (p == nullptr) return;

    // Calculamos todas las casillas dentro del radio de movimiento
    int radio = p->radioMovimiento;
    for (int f = 0; f < 9; f++) {
        for (int c = 0; c < 9; c++) {
            if (f == fila && c == col) continue; // la propia casilla no cuenta

            int dist = abs(f - fila) + abs(c - col);
            if (dist <= radio) {
                // Solo mostramos casillas vacias o con enemigos (no aliados)
                Pieza* ocupante = tablero->getPieza(f, c);
                if (ocupante == nullptr || ocupante->getBando() != p->getBando()) {
                    movimientosDisponibles.push_back({ f, c });
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