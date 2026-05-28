#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "IAJugador.h"

enum class EstadoJuego {
    MENU,
    CARGANDO,
    JUGANDO_LOCAL,
    JUGANDO_IA,
    SELECCION_DIFICULTAD,
    VICTORIA,
    PAUSA,
    COMO_JUGAR,
    CONFIGURACION,
    CONFIRMAR_SALIDA,
    SALIR
};

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
    sf::Sprite spriteFondo;
    bool fondoCargado;

    // Icono de configuracion
    sf::Texture texConfiguracion;
    sf::Sprite spriteConfiguracion;
    bool configuracionCargada;
    sf::RectangleShape zonaConfiguracion;

    // Popup de confirmar salida
    sf::Texture texPlantaTriste;
    sf::Sprite spritePlantaTriste;
    bool plantaTristeCargada;
    int opcionSalidaSeleccionada; // 0 = SI, 1 = NO

    sf::Color colorBotonActivo;
    sf::Color colorBotonDeshabilitado;
    sf::Color colorBotonHover;
    sf::Color colorTexto;
    sf::Color colorTextoDeshabilitado;

    void dibujarTitulo();
    void dibujarBoton(const Boton& b, bool seleccionado);
    EstadoJuego confirmarBoton(int indice);

    int paginaComoJugar;
    static constexpr int TOTAL_PAGINAS = 4;
    void dibujarPaginaComoJugar(int pagina);

    // Configuracion
    int opcionConfiguracionSeleccionada;

    float volumenGeneral;
    float volumenMusica;
    float volumenEfectos;

    bool sonidoSilenciado;
    bool pantallaCompletaActivada;
    bool solicitudCambiarPantallaCompleta;

    static constexpr int TOTAL_OPCIONES_CONFIG = 7;

    void dibujarFilaConfiguracion(
        const std::string& nombre,
        const std::string& valor,
        float x,
        float y,
        bool seleccionada
    );

    void dibujarBarraVolumen(
        float valor,
        float x,
        float y,
        bool seleccionada
    );

    void restablecerConfiguracion();

public:
    Menu(sf::RenderWindow& vent, sf::Font& f);

    EstadoJuego procesarEvento(const sf::Event& event);
    void dibujar();

    EstadoJuego procesarEventoComoJugar(const sf::Event& event);
    void dibujarComoJugar();

    EstadoJuego procesarEventoDificultad(const sf::Event& event);
    void dibujarDificultad();

    EstadoJuego procesarEventoConfiguracion(const sf::Event& event);
    void dibujarConfiguracion();

    EstadoJuego procesarEventoConfirmarSalida(const sf::Event& event);
    void dibujarConfirmarSalida();

    // Volumen aplicado al AudioManager
    float getVolumenGeneral() const { return volumenGeneral; }

    float getVolumenMusicaAplicado() const {
        if (sonidoSilenciado) return 0.f;
        return volumenMusica * volumenGeneral / 100.f;
    }

    float getVolumenEfectosAplicado() const {
        if (sonidoSilenciado) return 0.f;
        return volumenEfectos * volumenGeneral / 100.f;
    }

    bool getSonidoSilenciado() const { return sonidoSilenciado; }

    bool getPantallaCompletaActivada() const {
        return pantallaCompletaActivada;
    }

    void setPantallaCompletaActivada(bool activa) {
        pantallaCompletaActivada = activa;
    }

    bool consumirSolicitudPantallaCompleta() {
        if (solicitudCambiarPantallaCompleta) {
            solicitudCambiarPantallaCompleta = false;
            return true;
        }
        return false;
    }

    Dificultad dificultadSeleccionada;
};
