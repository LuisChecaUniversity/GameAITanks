#include "TankManager.h"
#include "GameObject.h"
#include "BaseTank.h"
#include "ControlledTank.h"
#include "DumbTank.h"
#include "Tank_c008101c.h"
#include <SDL.h>
#include "TinyXML\tinyxml.h"
#include "Commons.h"
#include "Collisions.h"
#include "ObstacleManager.h"
#include <cassert>

//Initialise the instance to null.
TankManager* TankManager::mInstance = NULL;

//--------------------------------------------------------------------------------------------------

TankManager::TankManager()
{
	
}

//--------------------------------------------------------------------------------------------------

TankManager::~TankManager()
{
	for(unsigned int i = 0; i < mTanks.size(); i++)
		delete mTanks[i];
	mTanks.clear();
}

//--------------------------------------------------------------------------------------------------

TankManager* TankManager::Instance()
{
	if(!mInstance)
	{
		mInstance = new TankManager;
	}

	return mInstance;
}

//--------------------------------------------------------------------------------------------------

void TankManager::Init(SDL_Renderer* renderer)
{
	LoadTanks(renderer);
}

//--------------------------------------------------------------------------------------------------

void TankManager::UpdateTanks(float deltaTime, SDL_Event e)
{
	//Accumulate time and at set point give all living tanks a bonus.
	mAccumulatedTimeUntilBonusPoints += deltaTime;

	for(unsigned int i = 0; i < mTanks.size(); i++)
	{
		//Has the required survival time passed for a bonus?
		if(mAccumulatedTimeUntilBonusPoints > kSurvivalTimeUntilBonus)
			mTanks[i]->AddToScore(kScore_SurvivalBonus);

		mTanks[i]->Update(deltaTime, e);

		//If the health is below zero, delete this tank.
		if(mTanks[i]->GetHealth() <= 0)
			mTankIndicesToDelete.push_back(i);
	}

	//If the bonus time has passed then reset it.
	if(mAccumulatedTimeUntilBonusPoints > kSurvivalTimeUntilBonus)
		mAccumulatedTimeUntilBonusPoints = 0.0f;

	//Remove one Tank a frame.
	if(mTankIndicesToDelete.size() > 0)
	{
		mTanks.erase(mTanks.begin()+mTankIndicesToDelete[0]);
		mTankIndicesToDelete.erase(mTankIndicesToDelete.begin());
	}
}

//--------------------------------------------------------------------------------------------------

void TankManager::RenderTanks()
{
	for(unsigned int i = 0; i < mTanks.size(); i++)
		mTanks[i]->Render();
}

//--------------------------------------------------------------------------------------------------

void TankManager::LoadTanks(SDL_Renderer* renderer)
{
	string imagePath;

	//Get the whole xml document.
	TiXmlDocument doc;
	if(!doc.LoadFile(kTankPath))
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
		TankSetupDetails details;
				
		//Jump to the first 'tank' element - within 'data'
		for(TiXmlElement* tankElement = root->FirstChildElement("tank"); tankElement != NULL; tankElement = tankElement->NextSiblingElement())
		{
			details.StudentName			= tankElement->Attribute("studentName");
			details.TankType			= atoi(tankElement->Attribute("tankType"));
			details.TankImagePath		= tankElement->Attribute("tankPath");
			details.ManImagePath		= tankElement->Attribute("manPath");
			details.TurnRate			= (float)atof(tankElement->Attribute("turnRate"));
			details.StartPosition		= Vector2D((float)atof(tankElement->Attribute("x")), (float)atof(tankElement->Attribute("y")));
			details.Health				= atoi(tankElement->Attribute("health"));
			details.NumOfBullets		= atoi(tankElement->Attribute("bullets"));
			details.NumOfRockets		= atoi(tankElement->Attribute("rockets"));
			details.NumOfMines			= atoi(tankElement->Attribute("mines"));
			details.Fuel				= (float)atof(tankElement->Attribute("fuel"));
			details.Mass				= (float)atof(tankElement->Attribute("mass"));
			details.MaxSpeed			= (float)atof(tankElement->Attribute("maxspeed"));
			details.LeftCannonAttached	= (bool)atoi(tankElement->Attribute("leftCannon"));
			details.RightCannonAttached = (bool)atoi(tankElement->Attribute("rightCannon"));

			//Add the new waypoint with the read in details.
			mTanks.push_back(GetTankObject(renderer, details));
		}
	}
}

//--------------------------------------------------------------------------------------------------

BaseTank* TankManager::GetTankObject(SDL_Renderer* renderer, TankSetupDetails details)
{
	//Create a new tank of required type, but then cast it to BaseTank for the vector list.
	BaseTank* newBaseTank = NULL;

	if(details.StudentName == "ControlledTank")
	{
		ControlledTank* newControlledTank = new ControlledTank(renderer, details);
		newBaseTank = (BaseTank*)newControlledTank;
	}
	else if(details.StudentName == "DumbTank")
	{
		DumbTank* newDumbTank = new DumbTank(renderer, details);
		newBaseTank = (BaseTank*)newDumbTank;
	}
	else if(details.StudentName == "Tank_c008101c")
	{
		Tank_c008101c* newStudentTank = new Tank_c008101c(renderer, details);
		newBaseTank = (BaseTank*)newStudentTank;
	}

	//Assert if no tank was setup.
	assert(newBaseTank != NULL);

	//Return our new tank.
	return newBaseTank;
}

//--------------------------------------------------------------------------------------------------

void TankManager::CheckForCollisions()
{
	vector<GameObject*> listOfObjects = ObstacleManager::Instance()->GetObstacles();
	Vector2D tl, tr, bl, br;

	for(unsigned int i = 0; i < listOfObjects.size(); i++)
	{
		Rect2D rect = listOfObjects[i]->GetAdjustedBoundingBox();
		for(unsigned int j = 0; j < mTanks.size(); j++)
		{
			//mTanks[j]->GetCornersOfTank(&tl,&tr,&bl,&br);
			//if(Collisions::Instance()->PointInBox(tl, rect) || Collisions::Instance()->PointInBox(tr, rect) || 
			//   Collisions::Instance()->PointInBox(bl, rect) || Collisions::Instance()->PointInBox(bl, rect))
			if(Collisions::Instance()->PointInBox(mTanks[j]->GetPointAtFrontOfTank(), rect) ||
			   Collisions::Instance()->PointInBox(mTanks[j]->GetPointAtRearOfTank(), rect))
			{
				mTanks[j]->Rebound(listOfObjects[i]->GetCentralPosition());
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------

vector<BaseTank*>	TankManager::GetVisibleTanks(BaseTank* lookingTank)
{
	vector<BaseTank*> mVisibleTanks;

	for(unsigned int i = 0; i < mTanks.size(); i++)
	{
		//Don't test self.
		if(mTanks[i] != lookingTank)
		{
			//TODO: Not getting the correct heading???
			Vector2D heading = lookingTank->GetHeading();
			heading.Normalize();
			Vector2D vecToTarget = lookingTank->GetCentrePosition()-mTanks[i]->GetCentrePosition();
			double vecToTargetLength = vecToTarget.Length();

			//If tank is too far away then it can't be seen.
			if(vecToTargetLength < kFieldOfViewLength)
			{
				vecToTarget.Normalize();
				//cout << "Heading x = " << heading.x << " y = " << heading.y << endl;
				double dotProduct = heading.Dot(vecToTarget);
				//cout << "dot = " << dotProduct << endl;
				if(dotProduct > kFieldOfView)
				{
					Vector2D point1 = lookingTank->GetCentralPosition() + (vecToTarget*(vecToTargetLength*0.33f));
					Vector2D point2 = lookingTank->GetCentralPosition() + (vecToTarget*(vecToTargetLength*0.5f));
					Vector2D point3 = lookingTank->GetCentralPosition() + (vecToTarget*(vecToTargetLength*0.66f));

					//Tank is within fov, but is there a building in the way?
					for(unsigned int j = 0; j < ObstacleManager::Instance()->GetObstacles().size(); j++)
					{
						GameObject* currentObstacle = ObstacleManager::Instance()->GetObstacles().at(j);
		
						//Check if we have collided with this obstacle.
						if( !Collisions::Instance()->PointInBox(point1, currentObstacle->GetAdjustedBoundingBox()) &&
							!Collisions::Instance()->PointInBox(point2, currentObstacle->GetAdjustedBoundingBox()) &&
							!Collisions::Instance()->PointInBox(point3, currentObstacle->GetAdjustedBoundingBox()) )
						{
							mVisibleTanks.push_back(mTanks[i]);
							//cout << "Can see you!!" << endl;

							//Get out of the obstacle check.
							break;
						}
					}
				}
			}
		}
	}

	return mVisibleTanks;
}

//--------------------------------------------------------------------------------------------------