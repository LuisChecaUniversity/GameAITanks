#ifndef _GAMESCREENLEVEL1_H
#define _GAMESCREENLEVEL1_H

#include "GameScreen.h"
#include "Commons.h"
#include <SDL.h>
#include <vector>
using namespace::std;

class Texture2D;
class LevelMap;
class ObstacleManager;

class GameScreenLevel1 : GameScreen
{
//--------------------------------------------------------------------------------------------------
public:
	GameScreenLevel1(SDL_Renderer* renderer);
	~GameScreenLevel1();

	void Render();
	void Update(float deltaTime, SDL_Event e);

//--------------------------------------------------------------------------------------------------
private:
	Texture2D*				 mBackgroundTexture;
	LevelMap*				 mLevelMap;
};


#endif //_GAMESCREENLEVEL1_H