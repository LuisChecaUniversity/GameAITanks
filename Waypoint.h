#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "Commons.h"
#include <vector>
#include <SDL.h>
//#include <SDL_ttf.h>
using namespace::std;

//---------------------------------------------------------------
// A waypoint is a point on the map which information regarding where can be reached from this point.
// Used in pathfinding.

class Texture2D;

class Waypoint
{
	//---------------------------------------------------------------
public:
	Waypoint(SDL_Renderer* renderer, int id, Vector2D startPosition, vector<int> connectingIDs);
	~Waypoint();

	void		Render();
	int			GetID()										{return mID;}
	
	vector<int> GetConnectedWaypointIDs()					{return mConnectedWaypointIDs;}
	bool		IsConnectedTo(int waypointIDToCheck);

	Vector2D	GetPosition()								{return mPosition;}
	Texture2D*		mTexture;


	//---------------------------------------------------------------
private:
	SDL_Renderer*   mRenderer;
	//TTF_Font*		mFont;
	int				mID;
	Vector2D		mPosition;

	vector<int>	    mConnectedWaypointIDs;			
};

//---------------------------------------------------------------

#endif //WAYPOINT_H