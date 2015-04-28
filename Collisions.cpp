#include "Collisions.h"
#include "Commons.h"
#include <iostream>

//Initialise the instance to null.
Collisions* Collisions::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

Collisions::Collisions()
{
}

//--------------------------------------------------------------------------------------------------

Collisions::~Collisions()
{
	mInstance = NULL;
}

//--------------------------------------------------------------------------------------------------

Collisions* Collisions::Instance()
{
	if(!mInstance)
	{
		mInstance = new Collisions;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------
/*
bool Collisions::Circle(Character* character1, Character* character2)
{
	Vector2D vec = Vector2D( (character1->GetPosition().x - character2->GetPosition().x),
							 (character1->GetPosition().y - character2->GetPosition().y));
	double distance = sqrt((vec.x*vec.x)+(vec.y*vec.y));

	double combinedDistance = (character1->GetCollisionRadius()+character2->GetCollisionRadius());
	return distance < combinedDistance;
}		
*/
//--------------------------------------------------------------------------------------------------

//bool Collisions::Box(Character* character1, Character* character2)
//{
/*	Rect2D rect1 = character1->GetCollisionBox();
	Rect2D rect2 = character2->GetCollisionBox();

	if(rect1.x > (rect2.x+rect2.width))
		return false;
	else if((rect1.x+rect1.width) < rect2.x)
		return false;
	else if(rect1.y > (rect2.y+rect2.height))
		return false;
	else if((rect1.y+rect1.height) < rect2.y)
		return false;

	return true;
*/

//	return false;
//}

//--------------------------------------------------------------------------------------------------

bool Collisions::PointInBox(Vector2D point, Rect2D rect2)
{
	if( point.x > rect2.x &&
		point.x < (rect2.x+rect2.width) &&
		point.y > rect2.y &&
		point.y < (rect2.y+rect2.height) )
	{
		return true;
	}

	return false;
}

//--------------------------------------------------------------------------------------------------
