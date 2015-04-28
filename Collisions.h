#ifndef _COLLISIONS_H
#define _COLLISIONS_H

#include "Commons.h"

class Collisions
{
public:
	~Collisions();

	static Collisions* Instance();

	//bool Circle(Character* character1, Character* character2);
	//bool Box(Character* character1, Character* character2);
	bool PointInBox(Vector2D point, Rect2D rect2);

private:
	Collisions();

private:
	static Collisions* mInstance;

};

#endif //_COLLISIONS_H