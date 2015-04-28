#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Texture2D.h"
#include "Commons.h"
#include <SDL.h>

//---------------------------------------------------------------

class GameObject
{
	//---------------------------------------------------------------
public:
	GameObject(SDL_Renderer* renderer, GAMEOBJECT_TYPE typeOfGameObject, Vector2D startPosition, string imagePath);
	~GameObject();

	virtual void		Update(float deltaTime, SDL_Event e);
	virtual void		Render();

	virtual Rect2D		GetAdjustedBoundingBox();
	Vector2D			GetPosition()														{return mPosition;}
	Vector2D			GetCentralPosition();
	double				GetCollisionRadius()												{return mCollisionRadius;}

	GAMEOBJECT_TYPE		GetGameObjectType()													{return mGameObjectType;}
	//---------------------------------------------------------------
protected:
	Texture2D*	mTexture;
	Vector2D	mPosition;
	double		mRotationAngle;

	double		mCollisionRadius;

private:
	GAMEOBJECT_TYPE mGameObjectType;
};

//---------------------------------------------------------------

#endif //GAMEOBJECT_H