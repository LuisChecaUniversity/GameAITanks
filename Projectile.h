#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SDL.h>
#include "GameObject.h"

class BaseTank;

//--------------------------------------------------------------------------------------------------

class Projectile : public GameObject
{
public:
	Projectile(SDL_Renderer* renderer, ProjectileSetupDetails details, BaseTank* firer);
	~Projectile();

	void		Update(float deltaTime);
	void		Render();

	BaseTank*	GetFirer()									{return mFiringTank;}
	void		RegisterHit();

private:
	bool		mVisible;
	Vector2D	mDirection;
	double		mSpeed;
	BaseTank*	mFiringTank;
};

//--------------------------------------------------------------------------------------------------
#endif //Projectile_H