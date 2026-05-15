#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "IAJugador.h"

enum class EstadoJuego { MENU, CARGANDO, JUGANDO_LOCAL, JUGANDO_IA, SELECCION_DIFICULTAD, VICTORIA };

struct Boton {
    sf::RectangleShape forma;
    std::string texto;
    bool habilitado = false;
};

class Menu {
private:
    sf::RenderWindow& ventana;
    sf::Font& fuente;

    std::vector<Boton> botones;
    int botonSeleccionado;

    std::vector<Boton> botonesDificultad;
    void dibujarSubmenuDificultad();

    // Imagen de fondo del menu
    sf::Texture texFondo;
    sf::Sprite  spriteFondo;
    bool        fondoCargado;

    sf::Color colorBotonActivo;
    sf::Color colorBotonDeshabilitado;
    sf::Color colorBotonHover;
    sf::Color colorTexto;
    sf::Color colorTextoDeshabilitado;

    void dibujarTitulo();
    void dibujarBoton(const Boton& b, bool seleccionado);
    EstadoJuego confirmarBoton(int indice);

public:
    Menu(sf::RenderWindow& vent, sf::Font& f);

    EstadoJuego procesarEvento(const sf::Event& event);
    void dibujar();
    EstadoJuego procesarEventoDificultad(const sf::Event& event);
    void dibujarDificultad();
    Dificultad dificultadSeleccionada;
};