#ifndef _TANKMANAGER_H
#define _TANKMANAGER_H

//--------------------------------------------------------------------------------------------------
// Tank Manager is a class that keeps hold of all the Tanks in the scene.
// This is used to read in the tanks from a text file and for scoring purposes.

#include "Commons.h"
#include <SDL.h>
#include <vector>
using namespace::std;

class BaseTank;
class GameObject;

//--------------------------------------------------------------------------------------------------

class TankManager
{
	//---------------------------------------------------------------
public:
	~TankManager();
	static TankManager* Instance();

	void				Init(SDL_Renderer* renderer);

	//Note: You are not allowed to use this function in your version of an AI Tank - Any use will be deemed as cheating and result in disqualification - Use GetVisibleTanks() instead.
	vector<BaseTank*>	GetTanks()									{return mTanks;}

	vector<BaseTank*>	GetVisibleTanks(BaseTank* lookingTank);
	void				UpdateTanks(float deltaTime, SDL_Event e);
	void				RenderTanks();

	void				CheckForCollisions();

	//---------------------------------------------------------------
private:
	TankManager();
	void		LoadTanks(SDL_Renderer* renderer);
	BaseTank*	GetTankObject(SDL_Renderer* renderer, TankSetupDetails details);

	//---------------------------------------------------------------
private:
	static TankManager* mInstance;

	vector<BaseTank*>	mTanks;
	vector<int>			mTankIndicesToDelete;

	float			    mAccumulatedTimeUntilBonusPoints;
};

//--------------------------------------------------------------------------------------------------

#endif //_TANKMANAGER_H