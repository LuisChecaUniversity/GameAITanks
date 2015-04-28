#include "DumbTank.h"
#include "TankManager.h"
#include "C2DMatrix.h"

//--------------------------------------------------------------------------------------------------

DumbTank::DumbTank(SDL_Renderer* renderer, TankSetupDetails details) : BaseTank(renderer, details)
{
	Vector2D offset = Vector2D(0.0f, 250.0f);
	mPosition1		= details.StartPosition-offset;
	mPosition2		= details.StartPosition+offset;
}

//--------------------------------------------------------------------------------------------------

DumbTank::~DumbTank()
{
}

//--------------------------------------------------------------------------------------------------

void DumbTank::ChangeState(BASE_TANK_STATE newState)
{
	BaseTank::ChangeState(newState);
}

//--------------------------------------------------------------------------------------------------

void DumbTank::Update(float deltaTime, SDL_Event e)
{
	//This is a dumb tank. Do NOT copy this approach.

	//Did we see a tank?
	if(mTanksICanSee.size() == 0)
	{
		ChangeState(TANKSTATE_IDLE);

		//If there are no visible tanks, then keep moving.

		//Check if we reached position before turning.
		if(mPosition.y < mPosition1.y && mHeading.y != -1.0f)
		{
			mHeading = Vector2D(0.0f, -1.0f);
			mRotationAngle = 180.0f;
			mVelocity = Vector2D();
			return;
		}
		else if(mPosition.y > mPosition2.y && mHeading.y != 1.0f)
		{
			mHeading = Vector2D(0.0f, 1.0f);
			mRotationAngle = 0.0f;
			mVelocity = Vector2D();
			return;
		}
		else
		{
			//Move if we are facing the correct direction.
			mCurrentSpeed -= kSpeedIncrement*deltaTime;
			if(mCurrentSpeed < -GetMaxSpeed())
				mCurrentSpeed = -GetMaxSpeed();
		}
	}
	else
	{
		//Rotate man to face enemy tank.
		Vector2D toTarget = mTanksICanSee[0]->GetCentralPosition()-GetCentralPosition();
		toTarget.Normalize();
		double dot = toTarget.Dot(mManFireDirection);
		if(dot < 0.95f)
			RotateManByRadian(-0.05f, 1);

		//Otherwise stop moving and fire at the visible tank.
		mVelocity = Vector2D();
		ChangeState(TANKSTATE_MANFIRE);
	}

	BaseTank::Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void DumbTank::MoveInHeadingDirection(float deltaTime)
{
	//Get the force that propels in current heading.
	Vector2D force = (mHeading*mCurrentSpeed)-mVelocity;

	//Acceleration = Force/Mass
	Vector2D acceleration = force/GetMass();

	//Update velocity.
	mVelocity += acceleration * deltaTime;
	
	//Don't allow the tank does not go faster than max speed.
	mVelocity.Truncate(GetMaxSpeed()); //TODO: Add Penalty for going faster than MAX Speed.

	//Finally, update the position.
	Vector2D newPosition = GetPosition();
		newPosition.x += mVelocity.x*deltaTime;
		newPosition.y += (mVelocity.y/**-1.0f*/)*deltaTime;	//Y flipped as adding to Y moves down screen.
	SetPosition(newPosition);
}

//--------------------------------------------------------------------------------------------------