#ifndef _PICKUPMANAGER_H
#define _PICKUPMANAGER_H

// PickUp Manager is a singleton that keeps hold of all the pick ups in the scene.
// It collects its information from the XML file and can give useful information on request.

#include <SDL.h>
#include <vector>
using namespace::std;

class GameObject;
class BaseTank;

//--------------------------------------------------------------------------------------------------

class PickUpManager
{
	//---------------------------------------------------------------
public:
	~PickUpManager();

	static PickUpManager*		Instance();
	void						Init(SDL_Renderer* renderer);
	void						RenderPickUps();
	void						UpdatePickUps(float deltaTime);

	vector<GameObject*>			GetAllPickUps()								{return mPickups;}

	void						CheckForCollisions(vector<BaseTank*> listOfTanks);

	//---------------------------------------------------------------
private:
	PickUpManager();

	void LoadPickUps(SDL_Renderer* renderer);
	void CheckForACollision(BaseTank* tank);

	//---------------------------------------------------------------
private:
	static PickUpManager* mInstance;

	vector<GameObject*>   mPickups;
	vector<int>			  mPickUpIndicesToDelete;
};

//--------------------------------------------------------------------------------------------------

#endif //_PICKUPMANAGER_H