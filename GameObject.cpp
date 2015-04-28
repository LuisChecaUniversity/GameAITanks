#include "GameObject.h"
#include "Texture2D.h"
#include <SDL.h>
#include <iostream>

using namespace::std;

//--------------------------------------------------------------------------------------------------

GameObject::GameObject(SDL_Renderer* renderer, GAMEOBJECT_TYPE typeOfGameObject, Vector2D startPosition, string imagePath)
{
	mTexture = new Texture2D(renderer);
	mTexture->LoadFromFile(imagePath);

	mPosition		 = startPosition;
	mRotationAngle	 = 0.0f;
	mGameObjectType  = typeOfGameObject;

	mCollisionRadius = 100.0f;
}

//--------------------------------------------------------------------------------------------------

GameObject::~GameObject()
{
	delete mTexture;
	mTexture = NULL;
}

//--------------------------------------------------------------------------------------------------

void GameObject::Update(float deltaTime, SDL_Event e)
{
	cout << "GAMEOBJECT: Must override Update." << endl;
}

//--------------------------------------------------------------------------------------------------

void GameObject::Render()
{
	mTexture->Render(mPosition, mRotationAngle);
}

//--------------------------------------------------------------------------------------------------

Vector2D GameObject::GetCentralPosition()
{
	Vector2D centre = Vector2D(mPosition.x+(mTexture->GetWidth()*0.5f), mPosition.y+(mTexture->GetHeight()*0.5f));
	return centre;
}

//--------------------------------------------------------------------------------------------------

Rect2D GameObject::GetAdjustedBoundingBox()
{
	//Return texture size - Override for more precise collision.
	//This assumes there is only one image on the texture.
	return Rect2D(mPosition.x, mPosition.y, mTexture->GetWidth(), mTexture->GetHeight());
}

//--------------------------------------------------------------------------------------------------
