#ifndef _OBSTACLEMANAGER_H
#define _OBSTACLEMANAGER_H

//--------------------------------------------------------------------------------------------------
// Obstacle Manager is a class that keeps hold of all the obstacles in the scene.
// It collects its information from the XML file and can give useful information on request.

#include <SDL.h>
#include <vector>
using namespace::std;

class GameObject;

//--------------------------------------------------------------------------------------------------

class ObstacleManager
{
	//---------------------------------------------------------------
public:
	~ObstacleManager();

	static ObstacleManager* Instance();

	void				Init(SDL_Renderer* renderer);
	vector<GameObject*> GetObstacles()									{return mObstacles;}
	void				UpdateObstacles(float deltaTime, SDL_Event e);
	void				RenderObstacles();

	//---------------------------------------------------------------
private:
	ObstacleManager();

	void   LoadObstacles(SDL_Renderer* renderer);
	string GetImagePath(int id);

	//---------------------------------------------------------------
private:
	static ObstacleManager* mInstance;

	vector<GameObject*> mObstacles;
};

//--------------------------------------------------------------------------------------------------

#endif //_OBSTACLEMANAGER_H