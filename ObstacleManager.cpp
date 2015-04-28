#include "ObstacleManager.h"
#include "GameObject.h"
#include "ProjectileManager.h"
#include <SDL.h>
#include "TinyXML\tinyxml.h"

//Initialise the instance to null.
ObstacleManager* ObstacleManager::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

ObstacleManager::ObstacleManager()
{
}

//--------------------------------------------------------------------------------------------------

ObstacleManager::~ObstacleManager()
{
	for(unsigned int i = 0; i < mObstacles.size(); i++)
		delete mObstacles[i];
	mObstacles.clear();
}

//--------------------------------------------------------------------------------------------------

void ObstacleManager::Init(SDL_Renderer* renderer)
{
	LoadObstacles(renderer);
}

//--------------------------------------------------------------------------------------------------

ObstacleManager* ObstacleManager::Instance()
{
	if(!mInstance)
	{
		mInstance = new ObstacleManager;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

void ObstacleManager::UpdateObstacles(float deltaTime, SDL_Event e)
{
}

//--------------------------------------------------------------------------------------------------

void ObstacleManager::RenderObstacles()
{
	for(unsigned int i = 0; i < mObstacles.size(); i++)
		mObstacles[i]->Render();
}

//--------------------------------------------------------------------------------------------------

void ObstacleManager::LoadObstacles(SDL_Renderer* renderer)
{
	string imagePath;

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
		string imagePath;
				
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
					if(name == "Obstacle")
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
								if(name == "ID")
								{
									id = atoi(propertyElement->Attribute("value"));
								}
							}

							imagePath = GetImagePath(id);

							//Add the new obstacle with the read in details.
							mObstacles.push_back(new GameObject(renderer, GAMEOBJECT_OBSTACLE, Vector2D(x,y), imagePath));
						}		
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

string ObstacleManager::GetImagePath(int id)
{
	string path = "";

	switch(id)
	{
		case 0:
			path = "Images/Building1.png";
		break;

		case 1:
			path = "Images/Building2.png";
		break;

		case 2:
			path = "Images/Building3.png";
		break;

		case 3:
			path = "Images/Building4.png";
		break;

		case 4:
			path = "Images/Building5.png";
		break;
	}

	return path;
}

//--------------------------------------------------------------------------------------------------