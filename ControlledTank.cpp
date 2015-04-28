#include "ControlledTank.h"
#include "TankManager.h"
#include "Commons.h"
#include "C2DMatrix.h"

//--------------------------------------------------------------------------------------------------

ControlledTank::ControlledTank(SDL_Renderer* renderer, TankSetupDetails details) 
	: BaseTank(renderer, details)
{
	mTankTurnDirection  = DIRECTION_UNKNOWN;
	mTankTurnKeyDown	= false;
	mTankMoveDirection	= DIRECTION_NONE;
	mTankMoveKeyDown	= false;
	mManTurnDirection   = DIRECTION_UNKNOWN;
	mManKeyDown			= false;
	mFireKeyDown		= false;
}

//--------------------------------------------------------------------------------------------------

ControlledTank::~ControlledTank()
{
}

//--------------------------------------------------------------------------------------------------

void ControlledTank::ChangeState(BASE_TANK_STATE newState)
{
	BaseTank::ChangeState(newState);
}

//--------------------------------------------------------------------------------------------------

void ControlledTank::Update(float deltaTime, SDL_Event e)
{
	//TODO: Adjust mHeading.
	switch(e.type)
	{
		//Deal with keyboard input.
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym)
			{
				//Tank speed keys.
				case SDLK_UP:
					mTankMoveDirection	= DIRECTION_FORWARD;
					mTankMoveKeyDown	= true;
				break;

				case SDLK_DOWN:
					mTankMoveDirection	= DIRECTION_BACKWARD;
					mTankMoveKeyDown	= true;
				break;

				//Tank directional keys.
				case SDLK_LEFT:
					if(!mTankTurnKeyDown)
					{
						mTankTurnKeyDown	= true;
						mTankTurnDirection	= DIRECTION_LEFT;
					}
				break;

				case SDLK_RIGHT:
					if(!mTankTurnKeyDown)
					{
						mTankTurnKeyDown		= true;
						mTankTurnDirection	= DIRECTION_RIGHT;
					}
				break;

				//Man directional keys
				case SDLK_a:
					if(!mManKeyDown)
					{
						mManKeyDown		  = true;
						mManTurnDirection = DIRECTION_LEFT;
					}
				break;

				case SDLK_d:
					if(!mManKeyDown)
					{
						mManKeyDown		  = true;
						mManTurnDirection = DIRECTION_RIGHT;
					}
				break;

				//Fire input.
				case SDLK_1:
					ChangeState(TANKSTATE_MANFIRE);
				break;

				case SDLK_2:
					if(mCurrentState != TANKSTATE_CANNONFIRE)
						ChangeState(TANKSTATE_CANNONFIRE);
				break;
			}
		break;

		case SDL_KEYUP:
			switch(e.key.keysym.sym)
			{
				//Tank directional input.
				case SDLK_LEFT:
				case SDLK_RIGHT:
					mTankTurnKeyDown		= false;
					mTankTurnDirection	= DIRECTION_UNKNOWN;
				break;

				case SDLK_UP:
				case SDLK_DOWN:
					//mTankMoveKeyDown = false;
					//mTankMoveDirection = DIRECTION_NONE;
					//mCurrentSpeed = 0.0f;
					//mVelocity.Zero();
				break;

				//Man directional input.
				case SDLK_a:
				case SDLK_d:
					mManKeyDown			= false;
					mManTurnDirection	= DIRECTION_UNKNOWN;
				break;

				//Rocket input.
				case SDLK_1:
					ChangeState(TANKSTATE_IDLE);
				break;

				//Drop Mine input.
				case SDLK_3:
					ChangeState(TANKSTATE_DROPMINE);
				break;
			}
		break;
	}
	
	//Tank movement.
	if(mTankTurnKeyDown)
	{
		if(mTankTurnDirection == DIRECTION_LEFT)
			RotateHeadingByRadian(0.05f, -1);
		else if(mTankTurnDirection == DIRECTION_RIGHT)
			RotateHeadingByRadian(0.05f, 1);
	}
	if(mTankMoveKeyDown)
	{
		if(mTankMoveDirection == DIRECTION_FORWARD)
		{
			mCurrentSpeed -= kSpeedIncrement*deltaTime;
			if(mCurrentSpeed < -GetMaxSpeed())
				mCurrentSpeed = -GetMaxSpeed();
		}
		else if(mTankMoveDirection == DIRECTION_BACKWARD)
		{
			mCurrentSpeed += kSpeedIncrement*deltaTime;
			if(mCurrentSpeed > GetMaxSpeed())
				mCurrentSpeed = GetMaxSpeed();
		}
	}

	//Man movement.
	if(mManKeyDown)
	{
		if(mManTurnDirection == DIRECTION_LEFT)
			RotateManByRadian(-0.05f, 1);
		else if(mManTurnDirection == DIRECTION_RIGHT)
			RotateManByRadian(0.05f, 1);
	}

	//Call parent update.
	BaseTank::Update(deltaTime, e);
}

//--------------------------------------------------------------------------------------------------

void ControlledTank::MoveInHeadingDirection(float deltaTime)
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

void ControlledTank::RotateHeadingByRadian(double radian, int sign)
{
	//Clamp the amount to turn to the max turn rate.
	if (radian > mMaxTurnRate) 
		radian = mMaxTurnRate;
	else if(radian < -mMaxTurnRate)
		radian = -mMaxTurnRate;
	//IncrementTankRotationAngle(RadsToDegs(radian));
    mRotationAngle += RadsToDegs(radian)*sign;

	//Usee a rotation matrix to rotate the player's heading
	C2DMatrix RotationMatrix;
  
	//Calculate the direction of rotation.
	RotationMatrix.Rotate(radian * sign);	
	//Get the new heading.
	RotationMatrix.TransformVector2Ds(mHeading);

	//cout << "RotateHeadingByRadian -- Heading x = " << mHeading.x << " y = " << mHeading.y << endl;

	//Get the new velocity.
	RotationMatrix.TransformVector2Ds(mVelocity);

	//Side vector must always be perpendicular to the heading.
	mSide = mHeading.Perp();
}

//--------------------------------------------------------------------------------------------------