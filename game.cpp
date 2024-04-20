#include "Game.h"
#include <iostream>
#include<fstream>



Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& path)
{
	//TODO: read config file here
	//		use the premade PlayerConfig, EnemyConfig, BulletConfig variables
	std::ifstream fin(path);

	//set us default window parameters
	//m_window.create(sf::VideoMode(1280, 720), "Assignment 2");
	//m_window.setFramerateLimit(60);

	std::string temp;

	fin >> temp;
	fin >> m_WindowConfig.ww >> m_WindowConfig.wh >> m_WindowConfig.fr >> m_WindowConfig.ds;
	if(m_WindowConfig.ds)
		m_window.create(sf::VideoMode(m_WindowConfig.ww, m_WindowConfig.wh), "Assignment 2", sf::Style::Fullscreen);
	else
		m_window.create(sf::VideoMode(m_WindowConfig.ww, m_WindowConfig.wh), "Assignment 2", sf::Style::Default);

	m_window.setFramerateLimit(m_WindowConfig.fr);

	//setting up font

	fin >> temp;
	fin >> m_FontConfig.font >> m_FontConfig.size >> m_FontConfig.r >> m_FontConfig.g >> m_FontConfig.b;
	if (!m_font.loadFromFile(m_FontConfig.font))
	{
		std::cout << "Font not loaded\n";
	}

	m_text.setFont(m_font);
	m_text.setCharacterSize(m_FontConfig.size);
	m_text.setFillColor(sf::Color(m_FontConfig.r, m_FontConfig.g, m_FontConfig.b));
	//m_text.setString("hello world");

	//setting up playerConfig
	fin >> temp;
	fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.s >> m_playerConfig.FR >>
		m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB
		>> m_playerConfig.OT >> m_playerConfig.V;

	spawnPlayer();

	//setting up enemyConfig;
	fin >> temp;
	fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >>
		m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN
		>> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;

	//setting up bulletConfig;

	fin >> temp;
	fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.s >> m_bulletConfig.FR >> m_bulletConfig.FG
		>> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT
		>> m_bulletConfig.V >> m_bulletConfig.L;
}

void Game::run()
{
	//TODO: add pause fucntionality in here
	//		some system should function while paused(rendaring)
	//		some system shouldn't(movement / input)

	while (m_runing)
	{
		m_currentFrame++;
		m_entities.update();
		sEnemySpawner();
		sMovement();
		sCollision();
		sUserInput();
		sLifespan();
		sRender();

		

		// incremenr the current frame
		// may need to be moved when pause implemented
	}
}

void Game::setPaused(bool paused)
{
	//TODO
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
	// TODO: finish adding all properties of the player with the correct values from config

	// we create every entity by calling EntityManager.addEntity(tag)
	// this returns a std::shared_ptr<Entitry>, so we use auto to save typing
	auto entity = m_entities.addEntity("player");

	// Give this entity a transform so it spawns at (200,200) with velocity (1,1) and angle 0
	entity->cTransform = std::make_shared<CTransform>( Vec2(m_WindowConfig.ww / 2, m_WindowConfig.wh/2 ),
						Vec2(0, 0), 0);
	
	// the entity's shape will have radius 32, 8 sides, drark grey fill, and red outline of thickness 4
	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	// add an input component to the player so that we can use inputs
	entity->cInput = std::make_shared<CInput>();

	// since we want this entity to be our player, set our Game's player variable to be this Entity
	// this goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
	m_player = entity;
	//m_entities.addEntity("player");

	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
}

void Game::spawnEnemy()
{
	// TODO:	make sure te enemy is spawned properly with the m_enemyConfig variables
	//			the enmey must spawned completely within the bounds of the window
	//

	// record when the most recent enemy was spawned

	auto entity = m_entities.addEntity("enemy");

	Vec2 pos, velocity;
	int range;

	pos.x = m_enemyConfig.SR + (rand() % (m_WindowConfig.ww - 2 * m_enemyConfig.SR));
	pos.y = m_enemyConfig.SR + (rand() % (m_WindowConfig.wh - 2 * m_enemyConfig.SR));

	range = m_enemyConfig.VMAX - m_enemyConfig.VMIN;
	velocity.x = m_enemyConfig.VMIN + (rand() % range);
	velocity.y = m_enemyConfig.VMIN + (rand() % range);

	entity->cTransform = std::make_shared<CTransform>(pos, velocity , rand() % 9);

	// vertex of enemy should be in range (3-8)
	range = 8 - 3;
	int point = 3 + rand() % range;

	sf::Color fill_color(rand() % 255, rand() % 255, rand() % 255);

	entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, point,
		fill_color,
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);

	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

	m_lastEnemySpawnTime = m_currentFrame;
}

// record the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	// TODO: span small enemies at the location of the input enemy e

	// when we create the smaller enemy, we have to read the values of the original enemu
	// - spawn a number of small enemies equal to the vertices of original enemy
	// - set each small enemy to the same color as the original, half the size
	// - small enemies are worth double points of the original enemy

	int n_of_sEnemy = e->cShape->circle.getPointCount();
	//std::cout << n_of_sEnemy << "\n";
	float angle = 2*pi / n_of_sEnemy;
	for (int i = 0; i < n_of_sEnemy; i++)
	{
		auto entity = m_entities.addEntity("small_enemy");
		Vec2 pos, velocity = { m_enemyConfig.SMAX, m_enemyConfig.SMAX };
		pos		 = e->cTransform->pos;
		velocity.x = velocity.x * cos(angle * i);
		velocity.y = velocity.y * sin(angle * i);

		entity->cTransform  = std::make_shared<CTransform>(pos, velocity, 4);
		entity->cShape		= e->cShape;
		entity->cShape->circle.setRadius(m_enemyConfig.SR / 2);
		entity->cCollision	= std::make_shared<CCollision>(m_enemyConfig.SR / 2);
		entity->cLifespan	= std::make_shared<CLifespan>(m_enemyConfig.L);
	}
}

// spawn a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity>entity, const Vec2& target)
{
	entity->cTransform = std::make_shared<CTransform>(Vec2(m_player->cTransform->pos.x, m_player->cTransform->pos.y),
		Vec2(0, 0), 0);

	entity->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);

	entity->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

	entity->cLifespan = std::make_shared<CLifespan> (m_bulletConfig.L);
	
	entity->cInput = std::make_shared<CInput>();

	entity->cInput->shoot_direc = target;

	entity->cInput->shoot = true;

}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> e)
{
	int n_of_bullet = 2 * m_playerConfig.V;
	//std::cout << n_of_sEnemy << "\n";
	float angle = 2 * pi / n_of_bullet;
	for (int i = 0; i < n_of_bullet; i++)
	{
		auto entity = m_entities.addEntity("special_bullet");
		Vec2 pos, velocity ;
		pos = e->cTransform->pos;
		velocity.x = m_bulletConfig.s * cos(angle * i);
		velocity.y = m_bulletConfig.s * sin(angle * i);

		entity->cTransform = std::make_shared<CTransform>(pos, velocity, 4);

		entity->cShape = std::make_shared<CShape>(m_bulletConfig.SR/2, m_bulletConfig.V,
			sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
			sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);

		entity->cCollision = std::make_shared<CCollision>(m_bulletConfig.SR / 2);

		entity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L/4);

	}
}

void Game::sMovement()
{
	// TODO:	implement all entity movement in this fucntion
	//			you should read the m_player-> cInput component to determine if the player is moving

	// sample movement speed update
	// 
	// 
	// 
	
	// player_movements
	{
		{
			m_player->cTransform->velocity.x = 0;
			m_player->cTransform->velocity.y = 0;

			if (m_player->cInput->up)
			{
				m_player->cTransform->velocity.y -= m_playerConfig.s;
			}
			if (m_player->cInput->down)
			{
				m_player->cTransform->velocity.y += m_playerConfig.s;
			}
			if (m_player->cInput->left)
			{
				m_player->cTransform->velocity.x -= m_playerConfig.s;
			}
			if (m_player->cInput->right)
			{
				m_player->cTransform->velocity.x += m_playerConfig.s;
			}

			//player must not move beyond the window

			m_player->cTransform->pos.x += m_player->cTransform->velocity.x;

			if (m_player->cTransform->pos.x < m_playerConfig.SR)
			{
				m_player->cTransform->pos.x = m_playerConfig.SR;
			}

			if (m_player->cTransform->pos.x > (m_WindowConfig.ww - m_playerConfig.SR))
			{
				m_player->cTransform->pos.x = m_WindowConfig.ww - m_playerConfig.SR;
			}

			m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

			if (m_player->cTransform->pos.y < m_playerConfig.SR)
			{
				m_player->cTransform->pos.y = m_playerConfig.SR;
			}

			if (m_player->cTransform->pos.y > (m_WindowConfig.wh - m_playerConfig.SR))
			{
				m_player->cTransform->pos.y = m_WindowConfig.wh - m_playerConfig.SR;
			}
		}
	}

	// bullet_movement
	{
		for (auto a : m_entities.getEntities("bullet"))
		{
			if (a->cInput->shoot)
			{
				a->cTransform->velocity = (a->cInput->shoot_direc - m_player->cTransform->pos).normalize();
				a->cTransform->velocity *= m_bulletConfig.s;
				a->cInput->shoot = false;
			}

			a->cTransform->pos += a->cTransform->velocity;
		}
	}

	// enemy_movement

	{
		for (auto& a : m_entities.getEntities("enemy"))
		{
			if ((a->cTransform->pos.x <= m_enemyConfig.SR) || (a->cTransform->pos.x >= (m_WindowConfig.ww - m_enemyConfig.SR)))
			{
				a->cTransform->velocity.x *= -1;
			}

			if ((a->cTransform->pos.y <= m_enemyConfig.SR) || (a->cTransform->pos.y >= (m_WindowConfig.wh - m_enemyConfig.SR)))
			{
				a->cTransform->velocity.y *= -1;
			}
			a->cTransform->pos += a->cTransform->velocity;
			a->cTransform->angle += 8.0f;
		}
	}

	// small_enemy_movement

	{
		for (auto& a : m_entities.getEntities("small_enemy"))
		{
			if ((a->cTransform->pos.x <= a->cShape->circle.getRadius()) || (a->cTransform->pos.x >= (m_WindowConfig.ww - a->cShape->circle.getRadius())))
			{
				a->cTransform->velocity.x *= -1;
			}

			if ((a->cTransform->pos.y <= a->cShape->circle.getRadius()) || (a->cTransform->pos.y >= (m_WindowConfig.wh - a->cShape->circle.getRadius())))
			{
				a->cTransform->velocity.y *= -1;
			}
			a->cTransform->pos += a->cTransform->velocity;
			a->cTransform->angle += 0.0f;
		}
	}

	//special_bullet

	{
		for (auto& a : m_entities.getEntities("special_bullet"))
		{
			a->cTransform->pos += a->cTransform->velocity;
		}
	}

}

void Game::sLifespan()
{
	// TODO: implement all lifespan functionality
	//
	// for all entities
	//	    if entity has no lifespan component, skip it
	//		if entity has > 0 remaing lifespan, subtract 1
	//		if it has lifespan and is alive
	//			scale its alpha channel properly
	//		if is has lifespan and its time is up
	//			destroy the entity 

	//bullet lifespan

	for (auto a : m_entities.getEntities("bullet"))
	{
		
		if (a->cLifespan->remaining <= 0) {
			a->destroy();
		}
		else
		{
			a->cLifespan->remaining -= 1;
		}
	}


	for (auto a : m_entities.getEntities("small_enemy"))
	{

		if (a->cLifespan->remaining <= 0) {
			a->destroy();
		}
		else
		{
			a->cLifespan->remaining -= 1;
		}
	}

	for (auto a : m_entities.getEntities("special_bullet"))
	{

		if (a->cLifespan->remaining <= 0) {
			a->destroy();
		}
		else
		{
			a->cLifespan->remaining -= 1;
		}
	}



}

void Game::sCollision()
{
	// TODO:	implement all proper collision between entities
	//			be sure to use collision radius, Not the shape radius

	// bullet and enemy, small_enemy collision
	for (auto& a : m_entities.getEntities("bullet"))
	{
		for (auto& e : m_entities.getEntities("enemy"))
		{

			if ((a->cTransform->pos - e->cTransform->pos).length() <= (a->cCollision->radius + e->cCollision->radius))
			{
				a->destroy();

				spawnSmallEnemies(e);

				e->destroy();
				m_score += 100;


			}
		}

		for (auto& e : m_entities.getEntities("small_enemy"))
		{

			if ((a->cTransform->pos - e->cTransform->pos).length() <= (a->cCollision->radius + e->cCollision->radius))
			{
				a->destroy();
				e->destroy();
				m_score += 200;
			}
		}
	}


	// enemy and player
	for (auto& a : m_entities.getEntities("enemy"))
	{
		/*if ((a->cTransform->pos - m_player->cTransform->pos).length() <= (a->cCollision->radius + m_player->cCollision->radius))
		{
			a->destroy();

			//e->destroy();
			m_player->destroy();
			m_lastPlayerspawnTime = m_currentFrame;
			m_playerRespawn = true;
		}

		if (((m_currentFrame - m_lastPlayerspawnTime) == m_playerRespawnTime) && m_playerRespawn)
		{
			m_playerRespawn = false;
			spawnPlayer();
		}

		std::cout << m_currentFrame << " " << m_lastPlayerspawnTime << " \n";*/

		if ((a->cTransform->pos - m_player->cTransform->pos).length() <= (a->cCollision->radius + m_player->cCollision->radius))
		{

			spawnSmallEnemies(a);
			a->destroy();
			m_player->destroy();
			m_score = 0;
			spawnPlayer();
		}

	}
	
	//special bullet , enemy, small enemy

	for (auto& a : m_entities.getEntities("special_bullet"))
	{
		for (auto& e : m_entities.getEntities("enemy"))
		{

			if ((a->cTransform->pos - e->cTransform->pos).length() <= (a->cCollision->radius + e->cCollision->radius))
			{
				a->destroy();

				spawnSmallEnemies(e);

				e->destroy();
				m_score += 100;


			}
		}

		for (auto& e : m_entities.getEntities("small_enemy"))
		{

			if ((a->cTransform->pos - e->cTransform->pos).length() <= (a->cCollision->radius + e->cCollision->radius))
			{
				a->destroy();
				e->destroy();
				m_score += 200;


			}
		}

	}
}

void Game::sEnemySpawner()
{
	// TODO:	code which implements enemy spawning should go there
	//			use m_currentFrame - m_lastEnemySpawnTime to determine
	//			how long it has been since the last enemy spawned

	if ((m_currentFrame - m_lastEnemySpawnTime) == m_WindowConfig.fr)
	{
		spawnEnemy();
	}
}

void Game::sRender()
{
	// TODO:	change the code below to draw all of the entities
	//			sample drawing of the player Entity that we have created


	m_window.clear();
	

	//std::cout << m_entities.getEntities().size() << "\n";

	// set the position of the shape based on the entitys transform->pos
	
	// draw the entity's sf::CircleShape

	// drawing player

	for (auto& a : m_entities.getEntities("player"))
	{
		
		a->cShape->circle.setPosition(a->cTransform->pos.x, a->cTransform->pos.y);

		//set the rotation of the shape based on the entity's transform-> angle
		a->cTransform->angle += 2.0f;
		a->cShape->circle.setRotation(a->cTransform->angle);
		m_window.draw(a->cShape->circle);
	}

	// drawing bullet
	for (auto& a : m_entities.getEntities("bullet"))
	{
		a->cShape->circle.setPosition(a->cTransform->pos.x, a->cTransform->pos.y);
		a->cTransform->angle += 0.0f;
		
		//color must change in a way that it will began to disappear as lifespan decrease
		
		float r = a->cLifespan->remaining, t = a->cLifespan->total, f;
		f = 255 * (r / t);


		a->cShape->circle.setFillColor(sf::Color(m_bulletConfig.FR, m_bulletConfig.FB,
			m_bulletConfig.FG , f));

		a->cShape->circle.setOutlineColor(sf::Color((m_bulletConfig.OR), m_bulletConfig.OG,
			m_bulletConfig.OR, f));
		a->cShape->circle.setRotation(a->cTransform->angle);
		m_window.draw(a->cShape->circle);
	}

	// drawing enemy

	for (auto& a : m_entities.getEntities("enemy"))
	{
		a->cShape->circle.setPosition(a->cTransform->pos.x, a->cTransform->pos.y);
		a->cTransform->angle += 5.0f;
		a->cShape->circle.setRotation(a->cTransform->angle);
		m_window.draw(a->cShape->circle);
	}

	// drawing small_enemy

	for (auto& a : m_entities.getEntities("small_enemy"))
	{
		a->cShape->circle.setPosition(a->cTransform->pos.x, a->cTransform->pos.y);
		a->cTransform->angle += 2.0f;
		
		//color must change in a way that it will began to disappear as lifespan decrease
		
		float r = a->cLifespan->remaining, t = a->cLifespan->total, f, w;
		f = 255 * (r / t); // highest alpha value * (r/t)

		//std::cout << f << " " << r << std::endl;
		sf::Color f_color( a->cShape->circle.getFillColor().r, a->cShape->circle.getFillColor().g,
			a->cShape->circle.getFillColor().b, f );

		a->cShape->circle.setFillColor(f_color);

		sf::Color o_color( a->cShape->circle.getOutlineColor().r , a->cShape->circle.getOutlineColor().g,
			a->cShape->circle.getOutlineColor().b, f);

		a->cShape->circle.setOutlineColor(o_color);
		a->cShape->circle.setRotation(a->cTransform->angle);
		m_window.draw(a->cShape->circle);
	}

	//drawing special_bullet

	for (auto& a : m_entities.getEntities("special_bullet"))
	{
		a->cShape->circle.setPosition(a->cTransform->pos.x, a->cTransform->pos.y);
		a->cTransform->angle += 0.0f;

		//color must change in a way that it will began to disappear as lifespan decrease

		float r = a->cLifespan->remaining, t = a->cLifespan->total, f;
		f = 255 * (r / t);


		a->cShape->circle.setFillColor(sf::Color(m_bulletConfig.FR, m_bulletConfig.FB,
			m_bulletConfig.FG, f));

		a->cShape->circle.setOutlineColor(sf::Color((m_bulletConfig.OR), m_bulletConfig.OG,
			m_bulletConfig.OR, f));
		a->cShape->circle.setRotation(a->cTransform->angle);
		m_window.draw(a->cShape->circle);
	}

	// draw score
	{
		std::string text = "Score = " + std::to_string(m_score);
		m_text.setString(text);
		m_text.setCharacterSize(m_FontConfig.size);
		m_text.setFillColor(sf::Color(m_FontConfig.r, m_FontConfig.g, m_FontConfig.b));
		m_window.draw(m_text);
	}

	m_window.display();
}

void Game::sUserInput()
{
	//TODO: handle user input here
	//		note that you should only be setting the player's input component variables here
	//		you should not implement the player's movement logic here;
	//		the movement system will read the variables you set in this function

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_runing = false;
		}

		// this event is triggerd when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W key pressed\n";
				m_player->cInput->up = true;
				break;
	
			case sf::Keyboard::S:
				std::cout << "S key pressed\n";
				m_player->cInput->down = true;
				break;

			case sf::Keyboard::A:
				std::cout << "A key pressed\n";
				m_player->cInput->left = true;
				break;

			case sf::Keyboard::D:
				std::cout << "D key pressed\n";
				m_player->cInput->right = true;
				break;
			default: break;
			}
		}

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				std::cout << "W key released\n";
				m_player->cInput->up = false;
				break;

			case sf::Keyboard::S:
				std::cout << "S key released\n";
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::A:
				std::cout << "A key released\n";
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::D:
				std::cout << "D key released\n";
				m_player->cInput->right = false;
				break;
			default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{

				spawnBullet(m_entities.addEntity("bullet"), Vec2(event.mouseButton.x,event.mouseButton.y) );

				std::cout << "left mouse butten clicked at (" << event.mouseButton.x << "," << event.mouseButton.y << ")\n";
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				spawnSpecialWeapon(m_player);
			}
		}
	}
}
