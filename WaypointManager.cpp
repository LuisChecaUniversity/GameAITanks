#include "WaypointManager.h"
#include <SDL.h>
#include <iostream>
#include "Waypoint.h"
#include "TinyXML\tinyxml.h"

//Initialise the instance to null.
WaypointManager* WaypointManager::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

WaypointManager::WaypointManager()
{
}

//--------------------------------------------------------------------------------------------------

WaypointManager::~WaypointManager()
{
	mInstance = NULL;

	for(unsigned int i = 0; i < mWaypoints.size(); i++)
		delete mWaypoints[i];
	mWaypoints.clear();
}

//--------------------------------------------------------------------------------------------------

WaypointManager* WaypointManager::Instance()
{
	if(!mInstance)
	{
		mInstance = new WaypointManager;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

void WaypointManager::Init(SDL_Renderer* renderer)
{
	mInstance->LoadWaypoints(renderer);
}

//--------------------------------------------------------------------------------------------------

void WaypointManager::RenderWaypoints()
{
#ifdef WAYPOINTS_VISIBLE
	for(unsigned int i = 0; i < mWaypoints.size(); i++)
	{
		mWaypoints[i]->Render();
	}
#endif
}

//--------------------------------------------------------------------------------------------------

void WaypointManager::LoadWaypoints(SDL_Renderer* renderer)
{
	//Get the whole xml document.
	TiXmlDocument doc;
	if(!doc.LoadFile(kTilemapPath))
	{
		cerr << doc.ErrorDesc() << endl;
	}

	//Now get the root element.
	TiXmlElement* root = doc.FirstChildElement();
	if(root == NULL)
	{
		cerr << "Failed to load file: No root element." << endl;
		doc.Clear();
	}
	else
	{
		float x = 0;
		float y = 0;
		int id = -1;
		vector<int> connections;
		
		//Jump to the first 'objectgroup' element.
		for(TiXmlElement* groupElement = root->FirstChildElement("objectgroup"); groupElement != NULL; groupElement = groupElement->NextSiblingElement())
		{
			string name = groupElement->Attribute("name");
			if(name == "WaypointLayer")
			{
				//Jump to the first 'object' element - within 'objectgroup'
				for(TiXmlElement* objectElement = groupElement->FirstChildElement("object"); objectElement != NULL; objectElement = objectElement->NextSiblingElement())
				{
					string name = objectElement->Attribute("name");
					if(name == "Waypoint")
					{
						x = (float)atof(objectElement->Attribute("x"));
						y = (float)atof(objectElement->Attribute("y"));

						//Jump to the first 'properties' element - within 'object'
						for(TiXmlElement* propertiesElement = objectElement->FirstChildElement("properties"); propertiesElement != NULL; propertiesElement = propertiesElement->NextSiblingElement())
						{
							//Loop through the 'property' elements - within 'properties'
							for(TiXmlElement* propertyElement = propertiesElement->FirstChildElement("property"); propertyElement != NULL; propertyElement = propertyElement->NextSiblingElement())
							{	
								string name = propertyElement->Attribute("name");
								if(name == "Connections")
								{
									//Split up the comma delimited connections.
									stringstream ss(propertyElement->Attribute("value"));
									int i;
									while(ss >> i)
									{
										connections.push_back(i);

										if(ss.peek() == ',')
											ss.ignore();
									}									
								}
								else if(name == "ID")
								{
									id = atoi(propertyElement->Attribute("value"));
								}
							}

							//Add the new waypoint with the read in details.
							mWaypoints.push_back(new Waypoint(renderer, id, Vector2D(x,y), connections));
							connections.clear();
						}		
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------


Waypoint* WaypointManager::GetWaypointWithID(int id)
{
	//Search for this waypoint and return it.
	for(unsigned int i = 0; i < mWaypoints.size(); i++)
	{
		if(mWaypoints[i]->GetID() == id)
			return mWaypoints[i];
	}

	//If we reach here there was no such waypoint.
	return NULL;
}

//--------------------------------------------------------------------------------------------------
