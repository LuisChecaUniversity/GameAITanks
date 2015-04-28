#include "ProjectileManager.h"
#include <SDL.h>
#include <iostream>
#include "Projectile.h"
#include "BaseTank.h"
#include "Collisions.h"
#include <algorithm>
#include "Commons.h"

//Initialise the instance to null.
ProjectileManager* ProjectileManager::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

ProjectileManager* ProjectileManager::Instance()
{
	if(!mInstance)
	{
		mInstance = new ProjectileManager;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

ProjectileManager::ProjectileManager()
{
}

//--------------------------------------------------------------------------------------------------

ProjectileManager::~ProjectileManager()
{
	mInstance = NULL;

	for(unsigned int i = 0; i < mProjectiles.size(); i++)
		delete mProjectiles[i];
	mProjectiles.clear();
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::UpdateProjectiles(float deltaTime)
{
	//Update the projectiles.
	for(unsigned int i = 0; i < mProjectiles.size(); i++)
		mProjectiles[i]->Update(deltaTime);

	//Check if any projectile have left the screen.
	Rect2D screenBox(0.0f, 0.0f, kScreenWidth, kScreenHeight);
	for(unsigned int i = 0; i < mProjectiles.size(); i++)
	{
		if(!Collisions::Instance()->PointInBox(mProjectiles[i]->GetCentralPosition(), screenBox))
		{
			//Prepare this projectile for deletion.
			if(std::find(mProjectileIndicesToDelete.begin(), mProjectileIndicesToDelete.end(), i) == mProjectileIndicesToDelete.end())
				mProjectileIndicesToDelete.push_back(i);
		}
	}

	//Remove one projectile a frame.
	if(mProjectileIndicesToDelete.size() > 0)
	{
		mProjectiles.erase(mProjectiles.begin()+mProjectileIndicesToDelete[0]);
		mProjectileIndicesToDelete.erase(mProjectileIndicesToDelete.begin());
	}
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::RenderProjectiles()
{
	for(unsigned int i = 0; i < mProjectiles.size(); i++)
		mProjectiles[i]->Render();
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::CheckForCollisions(vector<BaseTank*> listOfObjects)
{
	for(unsigned int i = 0; i < listOfObjects.size(); i++)
	{
		GameObject* gameObject = (GameObject*)listOfObjects[i];
		CheckForACollision(gameObject);
	}
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::CheckForCollisions(vector<GameObject*> listOfObjects)
{
	for(unsigned int i = 0; i < listOfObjects.size(); i++)
	{
		CheckForACollision(listOfObjects[i]);
	}
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::CheckForACollision(GameObject* gameObject)
{
	Rect2D rect = gameObject->GetAdjustedBoundingBox();
	for(unsigned int i = 0; i < mProjectiles.size(); i++)
	{
		if(Collisions::Instance()->PointInBox(mProjectiles[i]->GetPosition(), rect))
		{
			if(gameObject->GetGameObjectType() == GAMEOBJECT_TANK)
			{
				//Inform bullet that is has hit a target if it was a tank.
				if((GameObject*)mProjectiles[i]->GetFirer() == gameObject)
				{
					//Do nothing if this is a bullet from the firing tank.
					return;
				}
				else
				{
					//Damage Tank.
					BaseTank* tank = (BaseTank*)gameObject;
					tank->TakeDamage(mProjectiles[i]->GetGameObjectType());

					//Add score to firing tank.
					mProjectiles[i]->GetFirer()->AddToScore(kScore_BulletHit);

				}

				//Remove this projectile.
				mProjectiles[i]->RegisterHit();
			}
			
			//Prepare this bullet for deletion.
			if(std::find(mProjectileIndicesToDelete.begin(), mProjectileIndicesToDelete.end(), i) == mProjectileIndicesToDelete.end())
				mProjectileIndicesToDelete.push_back(i);
		}
	}
}

//--------------------------------------------------------------------------------------------------

void ProjectileManager::CreateProjectile(SDL_Renderer* renderer, ProjectileSetupDetails details, BaseTank* firer)
{
	switch(details.GameObjectType)
	{
		case GAMEOBJECT_BULLET:
			if(firer->GetBullets() > 0)
			{
				mProjectiles.push_back(new Projectile(renderer, details, firer));
				firer->DeductABullet();
			}
		break;

		case GAMEOBJECT_ROCKET:
			if(firer->GetRockets() > 0)
			{
				mProjectiles.push_back(new Projectile(renderer, details, firer));
				firer->DeductARocket();
			}
		break;

		case GAMEOBJECT_MINE:
			if(firer->GetMines() > 0)
			{
				mProjectiles.push_back(new Projectile(renderer, details, firer));
				firer->DeductAMine();
			}
		break;
	}
}

//--------------------------------------------------------------------------------------------------
