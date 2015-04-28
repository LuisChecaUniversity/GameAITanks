#ifndef _PROJECTILEMANAGER_H
#define _PROJECTILEMANAGER_H

//--------------------------------------------------------------------------------------------------
// Bullet Manager is a singleton that keeps hold of all the bullets in the scene.
// It is a singleton so that bullets can be created from anywhere.

#include <SDL.h>
#include "Commons.h"
#include <vector>
using namespace::std;

class Projectile;
class BaseTank;
class GameObject;

//--------------------------------------------------------------------------------------------------

class ProjectileManager
{
	//---------------------------------------------------------------
public:
	~ProjectileManager();

	static ProjectileManager* Instance();

	void CreateProjectile(SDL_Renderer* renderer, ProjectileSetupDetails details, BaseTank* firer);
	void UpdateProjectiles(float deltaTime);
	void RenderProjectiles();

	void CheckForCollisions(vector<BaseTank*> listOfObjects);
	void CheckForCollisions(vector<GameObject*> listOfObjects);

	//---------------------------------------------------------------
private:
	ProjectileManager();
	void CheckForACollision(GameObject* gameObject);

	//---------------------------------------------------------------
private:
	static ProjectileManager* mInstance;

	vector<Projectile*> mProjectiles;
	vector<int>			mProjectileIndicesToDelete;
};

//--------------------------------------------------------------------------------------------------

#endif //_PROJECTILEMANAGER_H