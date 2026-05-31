#include "Ranking.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <algorithm>

// Nombre del archivo donde se guardan las partidas
const std::string Ranking::ARCHIVO = "ranking.txt";

// Devuelve la fecha y hora actual en formato "DD/MM/AAAA HH:MM"
std::string Ranking::obtenerFechaHora() {
    std::time_t t = std::time(nullptr);
    std::tm* tm_local = std::localtime(&t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M", tm_local);
    return std::string(buf);
}

// Anade una nueva partida al final del archivo
void Ranking::guardarPartida(
    const std::string& ganador,
    const std::string& tipoVictoria,
    int turnos,
    int bajasLuz,
    int bajasOscuridad,
    int puntuacionLuz,
    int puntuacionOscuridad)
{
    // ios::app = abre el archivo en modo "anadir al final", lo crea si no existe
    std::ofstream archivo(ARCHIVO, std::ios::app);
    if (!archivo.is_open()) return;

    // Cada partida ocupa una linea, los campos separados por |
    archivo << ganador           << "|"
            << tipoVictoria      << "|"
            << turnos            << "|"
            << bajasLuz          << "|"
            << bajasOscuridad    << "|"
            << puntuacionLuz     << "|"
            << puntuacionOscuridad << "|"
            << obtenerFechaHora() << "\n";
}

// Lee el archivo y devuelve las ultimas maxEntradas partidas (mas recientes primero)
std::vector<EntradaRanking> Ranking::cargarPartidas(int maxEntradas) {
    std::vector<EntradaRanking> resultado;

    std::ifstream archivo(ARCHIVO);
    if (!archivo.is_open()) return resultado;

    std::string linea;
    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        // Separamos por | y rellenamos la struct
        std::istringstream ss(linea);
        std::string token;
        std::vector<std::string> campos;

        while (std::getline(ss, token, '|')) {
            campos.push_back(token);
        }

        if ((int)campos.size() < 8) continue;

        EntradaRanking e;
        e.ganador              = campos[0];
        e.tipoVictoria         = campos[1];
        e.turnosJugados        = std::stoi(campos[2]);
        e.bajasLuz             = std::stoi(campos[3]);
        e.bajasOscuridad       = std::stoi(campos[4]);
        e.puntuacionLuz        = std::stoi(campos[5]);
        e.puntuacionOscuridad  = std::stoi(campos[6]);
        e.fecha                = campos[7];

        resultado.push_back(e);
    }

    // Las mas recientes al principio
    std::reverse(resultado.begin(), resultado.end());

    if ((int)resultado.size() > maxEntradas) {
        resultado.resize(maxEntradas);
    }

    return resultado;
}
