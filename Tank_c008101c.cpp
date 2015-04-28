#include "Tank_c008101c.h"
#include "Collisions.h"
#include "Waypoint.h"
#include "ObstacleManager.h"
#include "PickUpManager.h"
#include "TankManager.h"
#include "WaypointManager.h"

//--------------------------------------------------------------------------------------------------
Tank_c008101c::Tank_c008101c(SDL_Renderer* renderer, TankSetupDetails details) : BaseTank(renderer, details)
{
	mAheadTex = new Texture2D(renderer);
	mAheadTex->LoadFromFile(kMinePath);

	mAhead2Tex = new Texture2D(renderer);
	mAhead2Tex->LoadFromFile(kMinePath);

	mTexCenter = Vector2D(mAheadTex->GetWidth(), mAheadTex->GetHeight()) * -0.5;

	mWaypointTex = WaypointManager::Instance()->GetAllWaypoints()[0]->mTexture;
	SetStateBehaviours();
}

//--------------------------------------------------------------------------------------------------
Tank_c008101c::~Tank_c008101c()
{
	delete mAheadTex;
	delete mAhead2Tex;
	mAheadTex = nullptr;
	mAhead2Tex = nullptr;
	mTargetWaypoint = nullptr;
	mWaypointTex;
	mPath.clear();
}

//--------------------------------------------------------------------------------------------------
void Tank_c008101c::CheckInput(SDL_Event e)
{
	switch (e.key.keysym.sym)
	{
	case SDLK_7:
		mBehaviour = STEERING_PATHFOLLOWING;
		break;
	case SDLK_8:
		mBehaviour = STEERING_SEEK;
		break;
	case SDLK_9:
		mBehaviour = STEERING_FLEE;
		break;
	case SDLK_0:
		mBehaviour = STEERING_ARRIVE;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------------------------------------------
void Tank_c008101c::Update(float deltaTime, SDL_Event e)
{
	CheckInput(e);

	if (e.button.state == SDL_PRESSED)
	{
		mFSM.ChangeState(pickups);
		//mTargetPosition = Vector2D(e.button.x, e.button.y);
		mNewTarget = true;
	}

	switch (mBehaviour)
	{
	case STEERING_SEEK:
		mSteeringForce = Seek(mTargetPosition);
		break;
	case STEERING_FLEE:
		mSteeringForce = Flee(mTargetPosition);
		break;
	case STEERING_ARRIVE:
		mSteeringForce = Arrive(mTargetPosition);
		break;
	case STEERING_PATHFOLLOWING:
		mSteeringForce = PathFinding(mTargetPosition);
		break;
	default:
		break;
	}

	BaseTank::Update(deltaTime, e);
	
	if (mVelocity.LengthSq() != 0)
	{
		RotateHeadingToFacePosition(GetCentralPosition() + mVelocity);
	}
}

//--------------------------------------------------------------------------------------------------
void Tank_c008101c::Render()
{
	BaseTank::Render();
	mAheadTex->Render(mAhead + mTexCenter, 0);
	mAhead2Tex->Render(mAhead2 + mTexCenter, 0);
}

//--------------------------------------------------------------------------------------------------
void Tank_c008101c::MoveInHeadingDirection(float deltaTime)
{
	Vector2D avoidance = ObstacleAvoidance();
	Vector2D force = avoidance + mSteeringForce;

	//Acceleration = Force/Mass
	Vector2D acceleration = force / GetMass();

	//Update velocity.
	mVelocity += acceleration * deltaTime;

	//Don't allow the tank does not go faster than max speed.
	mVelocity.Truncate(GetMaxSpeed()); //TODO: Add Penalty for going faster than MAX Speed.
	
	//Finally, update the position.
	Vector2D newPosition = GetPosition();
	newPosition += mVelocity * deltaTime;
	SetPosition(newPosition);
}

//--------------------------------------------------------------------------------------------------
Vector2D Tank_c008101c::Seek(Vector2D targetPosition)
{
	Vector2D origin = GetCentralPosition();
	origin = Vector2D(round(origin.x), round(origin.y));
	Vector2D resultingVelocity = Vec2DNormalize(targetPosition - origin) * GetMaxSpeed();

	return (resultingVelocity - mVelocity);
}

//--------------------------------------------------------------------------------------------------
Vector2D Tank_c008101c::Flee(Vector2D targetPosition)
{
	//only flee if the target is within 'panic distance'. Work in distance squared space.
	const double PanicDistanceSq = 100.0f * 100.0;
	if (Vec2DDistanceSq(GetPosition(), targetPosition) > PanicDistanceSq)
	{
		return Vector2D();
	}

	Vector2D resultingVelocity = Vec2DNormalize(GetPosition() - targetPosition) * GetMaxSpeed();

	return (resultingVelocity - mVelocity);
}

//--------------------------------------------------------------------------------------------------
Vector2D Tank_c008101c::Arrive(Vector2D targetPosition)
{
	Vector2D origin = GetCentralPosition();
	origin = Vector2D(round(origin.x), round(origin.y));
	Vector2D vectorToTarget = targetPosition - origin;

	//calculate the distance to the target
	double distance = vectorToTarget.Length();

	if (distance > 0)
	{
		const double decelerationFineTune = 1.0;

		double speed = distance / ((double)mDeceleration * decelerationFineTune);

		speed = min(speed, GetMaxSpeed());

		Vector2D resultingVelocity = vectorToTarget * speed / distance;
		resultingVelocity -= mVelocity;
		return resultingVelocity;
	}

	return Vector2D();
}

//--------------------------------------------------------------------------------------------------
Vector2D Tank_c008101c::ObstacleAvoidance()
{
	double dynamicLength = mVelocity.Length() / GetMaxSpeed();
	mAhead = GetCentralPosition() + Vec2DNormalize(mVelocity) * mMaxSeeAhead * dynamicLength;
	mAhead2 = GetCentralPosition() + Vec2DNormalize(mVelocity) * mMaxSeeAhead * 0.5 * dynamicLength;

	GameObject* mostThreatening = MostThreateningObstacle();

	if (mostThreatening != nullptr)
	{
		return Vec2DNormalize(mAhead - mostThreatening->GetCentralPosition()) * GetMaxSpeed();
	}

	return Vector2D();
}

//--------------------------------------------------------------------------------------------------
GameObject* Tank_c008101c::MostThreateningObstacle()
{
	GameObject* most = nullptr;

	for (GameObject* obstacle : ObstacleManager::Instance()->GetObstacles())
	{
		int offset = mTexture->GetWidth() / 2;
		Vector2D vOffset = Vector2D(offset, offset);
		Rect2D rect = obstacle->GetAdjustedBoundingBox();
		rect.x -= offset;
		rect.y -= offset;
		rect.height += offset;
		rect.width += offset;
		bool collision = Collisions::Instance()->PointInBox(mAhead + vOffset, rect)
			|| Collisions::Instance()->PointInBox(mAhead2 - vOffset, rect);

		Vector2D pos = GetPosition();
		if (collision && (most == nullptr || Vec2DDistance(pos, obstacle->GetCentralPosition()) < Vec2DDistance(pos, most->GetCentralPosition())))
		{
			most = obstacle;
		}
	}
	return most;
}

//--------------------------------------------------------------------------------------------------
Vector2D Tank_c008101c::PathFinding(Vector2D targetPosition)
{
	double dist = 0;
	double dist2 = 0;
	double dist3 = 0;
	double dist4 = 0;
	Waypoint* w;

	// Initial state: no target waypoint set
	if (mTargetWaypoint == nullptr && !mNewTarget)
	{
		return Vector2D();
	}
	// New target has been clicked
	if (mNewTarget)
	{
		// Reset waypoint texture
		if (mTargetWaypoint != nullptr)
		{
			mTargetWaypoint->mTexture = mWaypointTex;
		}
		mPath = mPathFinder.FindPath(GetCentralPosition(), targetPosition);

		if (!mPath.empty())
		{
			mTargetIndex = 0;
			w = mPath[mTargetIndex];
		}
		// Is new waypoint is target waypoint
		dist2 = Vec2DDistanceSq(GetCentralPosition(), w->GetPosition());
		dist3 = Vec2DDistanceSq(GetCentralPosition(), targetPosition);
		if (dist3 < dist2)
		{
			mMoveToClicked = true;
			return Arrive(targetPosition);
		}
		else
		{
			mTargetWaypoint = w;
			mNewTarget = false;
			mMoveToClicked = false;
		}
	}
	else if (mMoveToClicked)
	{
		return Arrive(targetPosition);
	}
	else if (mTargetWaypoint != nullptr)
	{
		dist = Vec2DDistanceSq(GetCentralPosition(), mTargetWaypoint->GetPosition());
		// Next waypoint if close enough
		if (dist <= 3600 && mTargetIndex + 1 < mPath.size())
		{
			w = mPath[++mTargetIndex];
		}
		else
		{
			return Seek(mTargetWaypoint->GetPosition());
		}

		// If had a target and have new target, check new target is better
		dist2 = Vec2DDistanceSq(w->GetPosition(), targetPosition);
		dist3 = Vec2DDistanceSq(GetCentralPosition(), w->GetPosition());
		dist4 = Vec2DDistanceSq(GetCentralPosition(), targetPosition);
		//Vec2DDistanceSq(GetCentralPosition(), mTargetWaypoint->GetPosition())
		
		if (dist4 < dist3 || dist4 < dist2)
		{
			mMoveToClicked = true;
			// Reset waypoint texture
			mTargetWaypoint->mTexture = mWaypointTex;
			return Arrive(targetPosition);
		}
		else //if (dist2 > dist)
		{
			// Reset waypoint texture
			mTargetWaypoint->mTexture = mWaypointTex;
			mTargetWaypoint = w;
		}
	}

	// New waypoint is null: exit, do nothing
	if (w == nullptr)
	{
		return Vector2D();
	}

	// Mark waypoint by changing texture
	mTargetWaypoint->mTexture = mAheadTex;
	// Move to target
	return Seek(mTargetWaypoint->GetPosition());
}

//--------------------------------------------------------------------------------------------------
Vector2D Tank_c008101c::GetNearestPickup()
{
	vector<GameObject*> allPickups = PickUpManager::Instance()->GetAllPickUps();
	GameObject* nearest = nullptr;
	double minDist = MaxDouble;
	double dist = 0;

	for (GameObject* pickup : allPickups)
	{
		dist = Vec2DDistanceSq(GetCentralPosition(), pickup->GetPosition());

		if (dist < minDist)
		{
			minDist = dist;
			nearest = pickup;
		}
	}

	return (nearest != nullptr) ? nearest->GetPosition() : Vector2D();
}

//--------------------------------------------------------------------------------------------------
void Tank_c008101c::Pickups()
{
	cout << "Pickup" << endl;
	Vector2D nearest = GetNearestPickup();

	if (nearest.isZero())
	{
		return;
	}

	mTargetPosition = nearest;
	mBehaviour = STEERING_ARRIVE;
}

//--------------------------------------------------------------------------------------------------
void Tank_c008101c::Patrol()
{
	//TankManager::Instance()->GetVisibleTanks(this);
	cout << "Patrol" << endl;
}

//--------------------------------------------------------------------------------------------------
void Tank_c008101c::AttackTank()
{
	cout << "Attack" << endl;
}

//--------------------------------------------------------------------------------------------------
void Tank_c008101c::PartesianRetreat()
{
	cout << "Partesian" << endl;
}

//--------------------------------------------------------------------------------------------------
void Tank_c008101c::Immolate()
{
	cout << "Immolate" << endl;
}

//--------------------------------------------------------------------------------------------------
void Tank_c008101c::SetStateBehaviours()
{
	// Function maps
	map<State, StateFuncPtr> enterMap;
	map<State, StateFuncPtr> exitMap;

	// Assign functions to states
	enterMap.emplace(patrol, &Tank_c008101c::Patrol);
	exitMap.emplace(patrol, &Tank_c008101c::Immolate);

	enterMap.emplace(pickups, &Tank_c008101c::Pickups);
	exitMap.emplace(pickups, &Tank_c008101c::Immolate);

	enterMap.emplace(attack, &Tank_c008101c::AttackTank);
	exitMap.emplace(attack, &Tank_c008101c::Immolate);

	enterMap.emplace(partesian, &Tank_c008101c::PartesianRetreat);
	exitMap.emplace(partesian, &Tank_c008101c::Immolate);
	
	// Allocate the maps in the FSM
	mFSM.SetEnterMap(enterMap);
	mFSM.SetExitMap(exitMap);
}
//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//
PathFinder::~PathFinder()
{
	for (auto it = edgeCosts.begin(); it != edgeCosts.end(); ++it)
	{
		delete *it;
	}
	for (auto it = openNodes.begin(); it != openNodes.end(); ++it)
	{
		delete *it;
	}
	for (auto it = closedNodes.begin(); it != closedNodes.end(); ++it)
	{
		delete *it;
	}

	edgeCosts.clear();
	openNodes.clear();
	closedNodes.clear();
}

//--------------------------------------------------------------------------------------------------
bool PathFinder::IsInList(vector<Node*> nodes, int waypointID)
{
	vector<Node*>::iterator it = find_if(nodes.begin(), nodes.end(), [waypointID](Node* n){ return n->waypoint->GetID() == waypointID; });
	return it != nodes.end();
}

//--------------------------------------------------------------------------------------------------
bool PathFinder::IsInList(vector<Node*> nodes, const Waypoint* const waypoint)
{
	vector<Node*>::iterator it = find_if(nodes.begin(), nodes.end(), [waypoint](Node* n){ return n->waypoint == waypoint; });
	return it != nodes.end();
}

//--------------------------------------------------------------------------------------------------
double PathFinder::GetCostFromWaypoints(const Waypoint* const from, const Waypoint* const to)
{
	for (EdgeCost* ec : edgeCosts)
	{
		if (ec->waypointFrom == from && ec->waypointTo == to)
		{
			return ec->cost;
		}
	}
	return MaxDouble;
}

//--------------------------------------------------------------------------------------------------
double PathFinder::GetHeuristictCost(const Vector2D& positionOne, const Vector2D& positionTwo)
{
	return Vec2DDistanceSq(positionOne, positionTwo);
}

//--------------------------------------------------------------------------------------------------
vector<Waypoint*> PathFinder::BuildPath(const Node* targetNode, const Vector2D& targetPosition)
{
	vector<Waypoint*> reversePath;

	const Node* currentNode = targetNode;
	//reversePath.push_back(targetPosition);
	while (currentNode != nullptr)
	{
		reversePath.push_back(currentNode->waypoint);
		currentNode = currentNode->parent;
	}
	// Path starts at end, needs to be reversed
	reverse(reversePath.begin(), reversePath.end());
	return reversePath;
}

//--------------------------------------------------------------------------------------------------
void PathFinder::SetEdgeCosts()
{
	Waypoint* toWaypoint = nullptr;

	for (Waypoint* w : WaypointManager::Instance()->GetAllWaypoints())
	{
		for (int connectedID : w->GetConnectedWaypointIDs())
		{
			toWaypoint = WaypointManager::Instance()->GetWaypointWithID(connectedID);
			edgeCosts.push_back(new EdgeCost(w, toWaypoint, 1));
		}
	}
}

//--------------------------------------------------------------------------------------------------
vector<Waypoint*> PathFinder::FindPath(Vector2D startPosition, Vector2D targetPosition)
{
	openNodes.clear();
	closedNodes.clear();

	vector<Waypoint*> path;
	Waypoint* nearestToStart = GetNearestWaypoint(startPosition);
	Waypoint* nearestToTarget = GetNearestWaypoint(targetPosition);

	if (nearestToStart == nearestToTarget || nearestToStart == nullptr || nearestToTarget == nullptr)
	{
		//path.push_back(targetPosition);
		return path;
	}

	openNodes.push_back(new Node(nearestToStart, nullptr, 0));

	Node* currentNode = nullptr;

	while (!openNodes.empty())
	{
		for (Node* n : openNodes)
		{
			if (currentNode == nullptr || n->cost < currentNode->cost)
			{
				currentNode = n;
			}

			if (currentNode->waypoint == nearestToTarget)
			{
				return BuildPath(currentNode, targetPosition);
			}
		}
		for (int id : currentNode->waypoint->GetConnectedWaypointIDs())
		{
			if (IsInList(openNodes, id) || IsInList(closedNodes, id))
			{
				continue;
			}
			Waypoint* connectedWaypoint = WaypointManager::Instance()->GetWaypointWithID(id);
			double cost = currentNode->cost + GetCostFromWaypoints(connectedWaypoint, currentNode->waypoint) + GetHeuristictCost(connectedWaypoint->GetPosition(), targetPosition);
			openNodes.push_back(new Node(connectedWaypoint, currentNode, cost));
		}
		// To advance the list, currentNode needs to be added in closed and removed from open
		closedNodes.push_back(currentNode);
		openNodes.erase(remove_if(openNodes.begin(), openNodes.end(), [currentNode](Node* p){ return p == currentNode; }));
		currentNode = nullptr;
	}

	return path;
}

//--------------------------------------------------------------------------------------------------
Waypoint* PathFinder::GetNearestWaypoint(const Vector2D& position)
{
	Waypoint* nearest = nullptr;
	double minDist = MaxDouble;
	double dist = 0;

	for (Waypoint* w : WaypointManager::Instance()->GetAllWaypoints())
	{
		dist = Vec2DDistanceSq(position, w->GetPosition());

		if (dist < minDist)
		{
			minDist = dist;
			nearest = w;
		}
	}

	return nearest;
}
//------------------------------------------------------------------------------------------------//