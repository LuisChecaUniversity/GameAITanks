#include "GameScreenLevel1.h"
#include <iostream>
#include "LevelMap.h"
#include "Texture2D.h"
#include "Collisions.h"
#include "WaypointManager.h"
#include "ObstacleManager.h"
#include "TankManager.h"
#include "ProjectileManager.h"
#include "PickUpManager.h"

using namespace::std;

//--------------------------------------------------------------------------------------------------

GameScreenLevel1::GameScreenLevel1(SDL_Renderer* renderer) : GameScreen(renderer)
{
	srand(NULL);
		
	//Set the level map.
	mLevelMap = new LevelMap(renderer);

	//Set up the waypoints.
	WaypointManager::Instance()->Init(renderer);

	//Set up the obstacles.
	ObstacleManager::Instance()->Init(renderer);

	//Set up the tanks.
	TankManager::Instance()->Init(renderer);

	//Set up PickUps
	PickUpManager::Instance()->Init(renderer);
}

//--------------------------------------------------------------------------------------------------

GameScreenLevel1::~GameScreenLevel1()
{
	//Level map.
	delete mLevelMap;
	mLevelMap = NULL;
}

//--------------------------------------------------------------------------------------------------

void GameScreenLevel1::Render()
{
	//Draw the background.
	mLevelMap->Render();

	//Draw the obstacles over the level map.
	ObstacleManager::Instance()->RenderObstacles();

	//Draw the bullets.
	ProjectileManager::Instance()->RenderProjectiles();

	//Draw the waypoints.
	WaypointManager::Instance()->RenderWaypoints();

	//Draw the pickUps.
	PickUpManager::Instance()->RenderPickUps();

	//Draw the tanks after everything else.
	TankManager::Instance()->RenderTanks();
}

//--------------------------------------------------------------------------------------------------

void GameScreenLevel1::Update(float deltaTime, SDL_Event e)
{
	switch(e.type)
	{
		//Deal with keyboard input.
		case SDL_KEYUP:
			switch(e.key.keysym.sym)
			{
				case SDLK_DOWN:
				break;

				default:
				break;
			}
		break;
	}

	//Update the obstacles.
	ObstacleManager::Instance()->UpdateObstacles(deltaTime, e);

	//Update the tanks.
	TankManager::Instance()->UpdateTanks(deltaTime, e);
	//Do Collision checks.
	//TankManager::Instance()->CheckForCollisions();

	//Update the bullets.
	ProjectileManager::Instance()->UpdateProjectiles(deltaTime);
	//Do collision checks.
	ProjectileManager::Instance()->CheckForCollisions(ObstacleManager::Instance()->GetObstacles());
	ProjectileManager::Instance()->CheckForCollisions(TankManager::Instance()->GetTanks());

	//Update the pickups.
	PickUpManager::Instance()->UpdatePickUps(deltaTime);
	PickUpManager::Instance()->CheckForCollisions(TankManager::Instance()->GetTanks());

}

//--------------------------------------------------------------------------------------------------