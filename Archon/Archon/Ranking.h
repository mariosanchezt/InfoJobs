#pragma once
#include <string>
#include <vector>

// Una entrada del historial: datos de una partida terminada
struct EntradaRanking {
    std::string ganador;          // "PLANTAS" o "ZOMBIES"
    std::string tipoVictoria;     // "ELIMINACION", "PUNTOS PODER", etc.
    int         turnosJugados;
    int         bajasLuz;
    int         bajasOscuridad;
    int         puntuacionLuz;
    int         puntuacionOscuridad;
    std::string fecha;            // "DD/MM/AAAA HH:MM"
};

// Clase con metodos estaticos para guardar y leer el historial
class Ranking {
public:
    // Guarda una partida al final del archivo
    static void guardarPartida(
        const std::string& ganador,
        const std::string& tipoVictoria,
        int turnos,
        int bajasLuz,
        int bajasOscuridad,
        int puntuacionLuz,
        int puntuacionOscuridad
    );

    // Carga las ultimas maxEntradas partidas (las mas recientes primero)
    static std::vector<EntradaRanking> cargarPartidas(int maxEntradas = 10);

private:
    static const std::string ARCHIVO;
    static std::string obtenerFechaHora();
};
