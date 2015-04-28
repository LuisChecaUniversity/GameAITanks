#ifndef _WAYPOINTMANAGER_H
#define _WAYPOINTMANAGER_H

//--------------------------------------------------------------------------------------------------
// Waypoint Manager is a singleton that keeps hold of all the waypoints in the scene.
// It collects its information from the XML file and can give useful information on request.

#include <SDL.h>
#include <vector>
using namespace::std;

class Waypoint;

//--------------------------------------------------------------------------------------------------

class WaypointManager
{
	//---------------------------------------------------------------
public:
	~WaypointManager();

	static WaypointManager* Instance();
	void					Init(SDL_Renderer* renderer);
	void					RenderWaypoints();

	Waypoint*				GetWaypointWithID(int id);
	vector<Waypoint*>		GetAllWaypoints()								{return mWaypoints;}

	//---------------------------------------------------------------
private:
	WaypointManager();

	void LoadWaypoints(SDL_Renderer* renderer);

	//---------------------------------------------------------------
private:
	static WaypointManager* mInstance;

	vector<Waypoint*> mWaypoints;
};

//--------------------------------------------------------------------------------------------------

#endif //_WAYPOINTMANAGER_H