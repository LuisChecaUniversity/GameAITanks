#include "PickUpManager.h"
#include <SDL.h>
#include <iostream>
#include <algorithm>
#include "GameObject.h"
#include "BaseTank.h"
#include "TinyXML\tinyxml.h"
#include "Collisions.h"

//Initialise the instance to null.
PickUpManager* PickUpManager::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

PickUpManager::PickUpManager()
{
}

//--------------------------------------------------------------------------------------------------

PickUpManager::~PickUpManager()
{
	mInstance = NULL;

	for(unsigned int i = 0; i < mPickups.size(); i++)
		delete mPickups[i];
	mPickups.clear();
}

//--------------------------------------------------------------------------------------------------

PickUpManager* PickUpManager::Instance()
{
	if(!mInstance)
	{
		mInstance = new PickUpManager;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::Init(SDL_Renderer* renderer)
{
	mInstance->LoadPickUps(renderer);
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::RenderPickUps()
{
	for(unsigned int i = 0; i < mPickups.size(); i++)
	{
		mPickups[i]->Render();
	}
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::UpdatePickUps(float deltaTime)
{
	//Remove one projectile a frame.
	if(mPickUpIndicesToDelete.size() > 0)
	{
		mPickups.erase(mPickups.begin()+mPickUpIndicesToDelete[0]);
		mPickUpIndicesToDelete.erase(mPickUpIndicesToDelete.begin());
	}
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::LoadPickUps(SDL_Renderer* renderer)
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
		GAMEOBJECT_TYPE type;
		
		//Jump to the first 'objectgroup' element.
		for(TiXmlElement* groupElement = root->FirstChildElement("objectgroup"); groupElement != NULL; groupElement = groupElement->NextSiblingElement())
		{
			string name = groupElement->Attribute("name");
			if(name == "ObjectLayer")
			{
				//Jump to the first 'object' element - within 'objectgroup'
				for(TiXmlElement* objectElement = groupElement->FirstChildElement("object"); objectElement != NULL; objectElement = objectElement->NextSiblingElement())
				{
					string name = objectElement->Attribute("name");
					if(name == "PickUp")
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
								if(name == "Type")
								{
									type = (GAMEOBJECT_TYPE)atoi(propertyElement->Attribute("value"));
								}
							}
						}
							
						//Add the new waypoint with the read in details.
						string path = "";
						switch(type)
						{
							case GAMEOBJECT_PICKUP_BULLETS:
								path = kBulletPickUpPath;
							break;

							case GAMEOBJECT_PICKUP_ROCKETS:
								path = kRocketPickUpPath;
							break;

							case GAMEOBJECT_PICKUP_HEALTH:
								path = kHealthPickUpPath;
							break;

							case GAMEOBJECT_PICKUP_MINE:
								path = kMinePickUpPath;
							break;
						}

						mPickups.push_back(new GameObject(renderer, type, Vector2D(x,y), path));	
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::CheckForCollisions(vector<BaseTank*> listOfTanks)
{
	for(unsigned int i = 0; i < listOfTanks.size(); i++)
	{
		CheckForACollision(listOfTanks[i]);
	}
}

//--------------------------------------------------------------------------------------------------

void PickUpManager::CheckForACollision(BaseTank* tank)
{
	Rect2D rect = tank->GetAdjustedBoundingBox();
	for(unsigned int i = 0; i < mPickups.size(); i++)
	{
		if(Collisions::Instance()->PointInBox(mPickups[i]->GetPosition(), rect))
		{
			if(tank->GetGameObjectType() == GAMEOBJECT_TANK)
			{
				//Prepare this pickUp for deletion.
				if(std::find(mPickUpIndicesToDelete.begin(), mPickUpIndicesToDelete.end(), i) == mPickUpIndicesToDelete.end())
					mPickUpIndicesToDelete.push_back(i);

				//Give the bonus to the colliding tank.
				switch(mPickups[i]->GetGameObjectType())
				{
					case GAMEOBJECT_PICKUP_BULLETS:
						tank->AddBullets(50);
						tank->AddToScore(kScore_PickUpBullets);
					break;

					case GAMEOBJECT_PICKUP_ROCKETS:
						tank->AddRockets(5);
						tank->AddToScore(kScore_PickUpRockets);
					break;

					case GAMEOBJECT_PICKUP_HEALTH:
						tank->AddHealth(100);
						tank->AddToScore(kScore_PickUpHealth);
					break;

					case GAMEOBJECT_PICKUP_MINE:
						tank->AddMines(3);
						tank->AddToScore(kScore_PickUpMines);
					break;
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------
