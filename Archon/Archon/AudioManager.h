#pragma once
#include <SFML/Audio.hpp>
#include <string>

class AudioManager {
private:
	sf::Music musicaMenu;
	sf::Music musicaJuego;

	sf::SoundBuffer bufferGolpe;
	sf::SoundBuffer bufferMuerte;
	sf::SoundBuffer bufferDisparo;

	sf::Sound sonidoGolpe;
	sf::Sound sonidoMuerte;
	sf::Sound sonidoDisparo;

	float volumenMusica;
	float volumenEfectos;

public:
	AudioManager();

	void cargar();

	void playMenuMusic();
	void playGameMusic();
	void stopMusic();

	void playGolpe();
	void playMuerte();
	void playDisparo();

	void setVolumenMusica(float vol);
	void setVolumenEfectos(float vol);
	float getVolumenMusica()const { return volumenMusica; }
	float getVolumenEfectos()const { return volumenEfectos; }
};