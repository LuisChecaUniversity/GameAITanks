#include "GameScreenManager.h"
#include "GameScreen.h"
#include "GameScreenLevel1.h"

//--------------------------------------------------------------------------------------------------

GameScreenManager::GameScreenManager(SDL_Renderer* renderer, SCREENS startScreen)
{
	mRenderer = renderer;
	mCurrentScreen = NULL;

	//Ensure the first screen is set up.
	ChangeScreen(startScreen);
}

//--------------------------------------------------------------------------------------------------

GameScreenManager::~GameScreenManager()
{
	mRenderer = NULL;

	delete mCurrentScreen;
	mCurrentScreen = NULL;
}

//--------------------------------------------------------------------------------------------------

void GameScreenManager::Render()
{
	mCurrentScreen->Render();
}

//--------------------------------------------------------------------------------------------------

void GameScreenManager::Update(float deltaTime, SDL_Event e)
{
	mCurrentScreen->Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void GameScreenManager::ChangeScreen(SCREENS newScreen)
{
	//Clear up the old screen.
	if(mCurrentScreen != NULL)
	{
		delete mCurrentScreen;
	}

	GameScreenLevel1* tempScreen1;

	//Initialise the new screen.
	switch(newScreen)
	{
		case SCREEN_INTRO:
		break;

		case SCREEN_MENU:
		break;

		case SCREEN_PLAYGROUND:
			tempScreen1 = new GameScreenLevel1(mRenderer);
			mCurrentScreen = (GameScreen*)tempScreen1;
			tempScreen1 = NULL;
		break;
		
		case SCREEN_ARENA:
		break;

		case SCREEN_GAMEOVER:
		break;
		
		case SCREEN_HIGHSCORES:
		break;
		
		default:
		break;
	}
}

//--------------------------------------------------------------------------------------------------