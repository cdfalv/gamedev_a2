#pragma once

#include "Entity.h"
#include "EntityManager.h"

#include <SFML/Graphics.hpp>

const float pi = 3.1416;

struct PlayerConfig	{ int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float s; };
struct EnemyConfig	{ int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; };
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float s; };
struct WindowConfig { int ww, wh, fr, ds; };
struct FontConfig { std::string font; int size, r, g, b; };

class Game
{
	sf::RenderWindow	m_window;		// the window we will draw to
	EntityManager		m_entities;		// vector of entities to maintain
	sf::Font			m_font;
	sf::Text			m_text;			// the score text to be drawn to the screen
	WindowConfig		m_WindowConfig;
	FontConfig			m_FontConfig;
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	int					m_score = 0;
	int					m_currentFrame = 0;
	int					m_lastEnemySpawnTime = 0;
	int					m_lastPlayerspawnTime = 0;
	int					m_playerRespawnTime = 60;
	bool				m_playerRespawn = false;
	bool				m_paused = false;	// whether we update game logic
	bool				m_runing = true;	//whether the game is running

	std::shared_ptr<Entity> m_player;
	void init(const std::string& config);	//initialize the gamestate with a config file path
	void setPaused(bool paused);			//pause the game

	void sMovement();						//system: Entity position / movement update
	void sUserInput();						//system: user input
	void sLifespan();						//system: lifespan
	void sRender();							//system: render/ / drawing
	void sEnemySpawner();					//system: spawns enimies
	void sCollision();						//system: collisions

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2 & mousePos);
	void spawnSpecialWeapon(std::shared_ptr<Entity> e);

public:

	Game(const std::string& config);	// constructor, takes in game config
	void run();

};
