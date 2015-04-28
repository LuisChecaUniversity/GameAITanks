#include "Projectile.h"
#include "BaseTank.h"
#include "Commons.h"
#include <iostream>

//--------------------------------------------------------------------------------------------------

Projectile::Projectile(SDL_Renderer* renderer, ProjectileSetupDetails details, BaseTank* firer)
	: GameObject(renderer, details.GameObjectType, details.StartPosition,  details.ImagePath)
{
	//Centralise the shot using half the texture dimensions.
	mPosition.x -= mTexture->GetWidth()*0.5f;
	mPosition.y -= mTexture->GetHeight()*0.5f;

	mVisible		= true;
	mDirection		= details.Direction;
	mRotationAngle	= details.RotationAngle;
	mFiringTank		= firer;

	switch(details.GameObjectType)
	{
		case GAMEOBJECT_BULLET:
			mSpeed = kBulletSpeed;
		break;

		case GAMEOBJECT_ROCKET:
			mSpeed = kRocketSpeed;
		break;

		case GAMEOBJECT_MINE:
			mSpeed = 0.0f;
		break;
	}
}

//--------------------------------------------------------------------------------------------------

Projectile::~Projectile()
{
	mFiringTank = NULL;
}

//--------------------------------------------------------------------------------------------------

void Projectile::Update(float deltaTime)
{
	mPosition += mDirection*mSpeed*deltaTime;
}

//--------------------------------------------------------------------------------------------------

void Projectile::Render()
{
	if(mVisible)
	{
		GameObject::Render();
	}
}

//--------------------------------------------------------------------------------------------------

void Projectile::RegisterHit()
{
	mVisible = false;
}

//--------------------------------------------------------------------------------------------------