#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "Constants.h"
#include "Commons.h"
#include "Texture2D.h"
#include "GameScreenManager.h"

using namespace::std;

//-----------------------------------------------------------------------------------------------------
//Local function prototypes.
bool InitSDL();
SDL_Surface* LoadSurface(string path);
void		 LoadMusic(string path);
void CloseSDL();

void Render();
bool Update();

//-----------------------------------------------------------------------------------------------------
//Globals.
SDL_Window*   gWindow    = NULL;
SDL_Renderer* gRenderer  = NULL;
SDL_Surface*  gSurface   = NULL;
Mix_Music*	  gMusic	 = NULL;
Uint32		  gOldTime;

GameScreenManager* gameScreenManager;

//-----------------------------------------------------------------------------------------------------

int main(int argc, char* args[])
{
	//Initialise SDL.
	if(InitSDL())
	{
		//Set up the game screen manager - Start with Level1
		gameScreenManager = new GameScreenManager(gRenderer, SCREEN_PLAYGROUND);
		
		//Start the music.
		//LoadMusic("Music/.mp3");
		if(Mix_PlayingMusic() == 0)
		{
			Mix_PlayMusic(gMusic, -1);
		}

		bool quit = false;
		gOldTime = SDL_GetTicks();

		//Game Loop.
		while(!quit)
		{
			Render();
			quit = Update();
		}	
	}

	//Close Window and free resources.
	CloseSDL();

	return 0;
}

//-----------------------------------------------------------------------------------------------------

bool InitSDL()
{	
	//Setup SDL.
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL did not initialise. Error: " << SDL_GetError();
		return false;
	}
	else
	{
		//Attempt to set texture filtering to linear.
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			cout << "Warning: Linear texture filtering not available";
		}

		//All good, so attempt to create the window.
		gWindow = SDL_CreateWindow("Game AI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kScreenWidth, kScreenHeight, SDL_WINDOW_SHOWN);
		//Did the window get created?
		if(gWindow != NULL)
		{
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if(gRenderer != NULL)
			{
				//Initialise PNG loading.
				int imageFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imageFlags) & imageFlags))
				{
					cout << "SDL_Image could not initialise. Error: " << IMG_GetError();
					return false;
				}
			}
			else
			{
				cout << "Renderer could not initialise. Error: " << SDL_GetError();
				return false;
			}
			
/*			//Initialise the Mixer.
			if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
			{
				cout << "Mixer could not initialise. Error: " << Mix_GetError();
				return false;
			}*/
		}
		else
		{
			//Nope.
			cout << "Window was not created. Error: " << SDL_GetError();
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------------------------------

SDL_Surface* LoadSurface(string path)
{
	SDL_Surface* pLoadedSurface = NULL;
	SDL_Surface* pOptimizedSurface = NULL;

	//Load the image.
	pLoadedSurface = IMG_Load(path.c_str());
	if(pLoadedSurface == NULL)
	{
		cout << "Failed to load image. Error: " << SDL_GetError();
		return NULL;
	}
	else
	{
		//Convert the surface to the screen format.
		pOptimizedSurface = SDL_ConvertSurface( pLoadedSurface, gSurface->format, NULL);
		if(pOptimizedSurface == NULL)
		{
			cout << "Unable to optimize the surface. Error: " << SDL_GetError();
			return NULL;
		}

		//Free the memory used for the loaded surface.
		SDL_FreeSurface(pLoadedSurface);
	}

	return pOptimizedSurface;
}

//-----------------------------------------------------------------------------------------------------

void LoadMusic(string path)
{
	gMusic = Mix_LoadMUS(path.c_str());
	if(gMusic == NULL)
	{
		cout << "Failed to load background music! Error: " << Mix_GetError() << endl;
	}
}

//-----------------------------------------------------------------------------------------------------

void CloseSDL()
{
	//Destroy the game screen manager.
	delete gameScreenManager;
	gameScreenManager = NULL;

	//Destroy the window.
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Release music.
	Mix_FreeMusic(gMusic);
	gMusic = NULL;

	//Quit SDL subsystems.
	IMG_Quit();
	SDL_Quit();
}

//-----------------------------------------------------------------------------------------------------

void Render()
{
	//Clear the screen.
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	gameScreenManager->Render();

	//Update the screen.
	SDL_RenderPresent(gRenderer);
}

//-----------------------------------------------------------------------------------------------------

bool Update()
{
	//Get the new time.
	Uint32 newTime = SDL_GetTicks();

	//Event Handler.
	SDL_Event e;

	//Get the events.
	SDL_PollEvent(&e);

	//Handle any events.
	switch(e.type)
	{
		//Click the 'X' to quit.
		case SDL_QUIT:
			return true;
		break;

		default:
			gameScreenManager->Update((float)(newTime-gOldTime)/1000.0f, e);
		break;
	}

	//Set the current time to be the old time.
	gOldTime = newTime;

	return false;
}

//-----------------------------------------------------------------------------------------------------