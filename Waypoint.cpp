#include "Waypoint.h"
#include "WaypointManager.h"
#include "Texture2D.h"
#include <SDL.h>
//#include <SDL_ttf.h>
#include <vector>

using namespace::std;
//---------------------------------------------------------------

Waypoint::Waypoint(SDL_Renderer* renderer, int id, Vector2D startPosition, vector<int> connectingIDs)
{
	mRenderer   = renderer;
	mID			= id;
	mPosition	= startPosition;

	mTexture = new Texture2D(renderer);
	mTexture->LoadFromFile(kWaypointPath);

	mConnectedWaypointIDs = connectingIDs;

    //mFont = TTF_OpenFont("ARIAL.ttf", 24);
}

//---------------------------------------------------------------

Waypoint::~Waypoint()
{
	mRenderer = NULL;

	mConnectedWaypointIDs.clear();
}

//---------------------------------------------------------------

void Waypoint::Render()
{
	//Draw lines to connecting nodes.
	for(unsigned int i = 0; i < mConnectedWaypointIDs.size(); i++)
	{
		Waypoint* connectedWaypoint = WaypointManager::Instance()->GetWaypointWithID(mConnectedWaypointIDs[i]);
		SDL_RenderDrawLine(mRenderer, (int)mPosition.x, (int)mPosition.y, (int)connectedWaypoint->GetPosition().x, (int)connectedWaypoint->GetPosition().y);
	}

	//Get a position of texture centred on mPosition.
	Vector2D dotPosition = mPosition;
	dotPosition.x -= mTexture->GetWidth()*0.5f;
	dotPosition.y -= mTexture->GetHeight()*0.5f;

	//Draw the waypoint.
	mTexture->Render(dotPosition);

	//Draw the id.
/*	if (mFont == NULL)
	{
		char id[2];
		sprintf_s(id, "%d", mID);

	   SDL_Surface *text;
	   SDL_Color text_color = {255, 255, 255};
	   text = TTF_RenderText_Solid(mFont, id, text_color);
	}*/
}

//---------------------------------------------------------------

bool Waypoint::IsConnectedTo(int waypointIDToCheck)
{
	//Check through the connect IDs to see if there is a match.
	for(unsigned int i = 0; i < mConnectedWaypointIDs.size(); i++)
	{
		if(mConnectedWaypointIDs[i] == waypointIDToCheck)
			return true;
	}

	//If we reach here there was no match.
	return false;
}

//---------------------------------------------------------------

