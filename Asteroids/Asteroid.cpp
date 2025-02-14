#include "Asteroid.h"
#include "CommonFiles/Renderer.h"
#include "Inih/cpp/INIReader.h"
#include "ClockHand.h"

#include <corecrt_math_defines.h>
#include <cmath>
#include <iostream>

const float Rad = (float)(M_PI / 180);

Asteroid::Asteroid(Renderer* _Rend, float x, float y, float Angle)
	:Rend(_Rend),
	Active(false),
	DeltaTime(0),
	Size(0),
	LastObjectHitted(-1)
{
	INIReader ConFile("InitialData.ini");

	if (ConFile.ParseError() < 0)
		ConFile.PrintError("Asteroid: ConFile Failed");

	OwnDimensions.Width = (float)ConFile.GetInteger("Asteroid", "Width", 1);
	OwnDimensions.Height = (float)ConFile.GetInteger("Asteroid", "Height", 1);
	Velocity = ConFile.GetInteger("Asteroid", "Velocity", 0);
	SpeedIncrease = ConFile.GetInteger("Asteroid", "SpeedIncrease", 0);
	ClockHandsNumber = ConFile.GetInteger("Asteroid", "ClockHandsNumber", 3);

	UpdateData(x, y, Angle);

	ClockHands = new ClockHand[ClockHandsNumber];
	for (i = 0; i < ClockHandsNumber; i++)
		ClockHands[i] = ClockHand(i, (OwnDimensions.Hypotenuse / 4.0f) * (i + 1), Rend);
}

void Asteroid::setBigAsteroid(float _Width, float _Height, float x, float y, int _Velocity)
{
	Active = true;
	Size = 0;
	OwnDimensions.Width = _Width;
	OwnDimensions.Height = _Height;
	HWidth = OwnDimensions.Width / 2.0f;
	HHeight = OwnDimensions.Height / 2.0f;
	FirstPoint.x = x;
	FirstPoint.y = y;
	LastObjectHitted = -1;
	Velocity = _Velocity;
	P1.x = cosf(FirstPoint.Rotation) * Velocity;
	P1.y = -sinf(FirstPoint.Rotation) * Velocity;
	Center.x = FirstPoint.x + HWidth;
	Center.y = FirstPoint.y + HHeight;
		
	OwnDimensions.Hypotenuse = sqrtf(powf(OwnDimensions.Width / 2.0f, 2) + powf(OwnDimensions.Height / 2.0f, 2));

	for(i =  0; i < ClockHandsNumber; i++)
		ClockHands[i].setSize((OwnDimensions.Hypotenuse / 4.0f) * (i + 1));
}

void Asteroid::Update(bool Pause, tm* CurrentTime)
{
	if (!Pause)
	{

		DeltaTime = Rend->getDeltaTime();

		FirstPoint.x += P1.x * DeltaTime;
		FirstPoint.y += P1.y * DeltaTime;
		Center.x = FirstPoint.x + HWidth;
		Center.y = FirstPoint.y + HHeight;

		if (FirstPoint.x + OwnDimensions.Width >= Rend->getWindowWidth())
		{
			FirstPoint.x = Rend->getWindowWidth() - OwnDimensions.Width - 5.0f;
			ChangeDirection(0); //"Wall" number
		}
		else if (FirstPoint.x <= 0)
		{
			FirstPoint.x = 5;
			ChangeDirection(2);
		}

		if (FirstPoint.y + OwnDimensions.Height >= Rend->getWindowHeight())
		{
			FirstPoint.y = Rend->getWindowHeight() - OwnDimensions.Height - 5.0f;
			ChangeDirection(1);
		}
		else if (FirstPoint.y <= 0)
		{
			FirstPoint.y = 5;
			ChangeDirection(3);
		}

	}

	Rend->DrawEmptyRect(&FirstPoint, &OwnDimensions, 255, 0, 0, 255);

	for (i = 0; i <  ClockHandsNumber; i++)
		ClockHands[i].Update(Pause, CurrentTime, &Center);

}

void Asteroid::setNewData(Position* Pos, int ParentSize, Dimension* NewDimensions, bool NewAsteroid, int ParentVelocity)
{
	Size = ParentSize + 1;
	OwnDimensions.Width = NewDimensions->Width / 2.0f;
	OwnDimensions.Height = NewDimensions->Height / 2.0f;
	Velocity = ParentVelocity + SpeedIncrease;

	if (NewAsteroid)
		UpdateData(Pos->x + OwnDimensions.Width, Pos->y - OwnDimensions.Height, Pos->Angle);
	else
		UpdateData(Pos->x - OwnDimensions.Width, Pos->y - OwnDimensions.Height, 360 - Pos->Angle);

	for (i = 0; i < ClockHandsNumber; i++)
		ClockHands[i].setSize((OwnDimensions.Hypotenuse / 4.0f) * (i + 1));
}

void Asteroid::UpdateData(float x, float y, float Angle)
{

	HWidth = OwnDimensions.Width / 2.0f;
	HHeight = OwnDimensions.Height / 2.0f;

	FirstPoint.x = x;
	FirstPoint.y = y;

	if (FirstPoint.x <= 1)
	{
		FirstPoint.x += OwnDimensions.Width;
		ChangeDirection(2);
	}
	else if (FirstPoint.x >= Rend->getWindowWidth())
	{
		FirstPoint.x -= OwnDimensions.Width;
		ChangeDirection(0);
	}

	if (FirstPoint.y <= 1)
	{
		FirstPoint.y += OwnDimensions.Height;
		ChangeDirection(3);
	}
	else if (FirstPoint.y >= Rend->getWindowHeight())
	{
		FirstPoint.y -= OwnDimensions.Height;
		ChangeDirection(1);
	}

	Center.x = FirstPoint.x + HWidth;
	Center.y = FirstPoint.y + HHeight;

	FirstPoint.Angle = Center.Angle = Angle;
	FirstPoint.Rotation = Center.Rotation = Angle * Rad;

	P1.x = cosf(FirstPoint.Rotation) * Velocity; 
	P1.y = -sinf(FirstPoint.Rotation) * Velocity; 

	OwnDimensions.Hypotenuse = sqrtf(powf(OwnDimensions.Width / 2.0f, 2) + powf(OwnDimensions.Height / 2.0f, 2));

}

void Asteroid::ChangeDirection(int ObjectNumber)
{
	if (ObjectNumber != LastObjectHitted)
	{
		FirstPoint.Angle += 180;
		FirstPoint.Rotation = FirstPoint.Angle * Rad;
		P1.x = cosf(FirstPoint.Rotation) * Velocity;
		P1.y = -sinf(FirstPoint.Rotation) * Velocity;
		LastObjectHitted = ObjectNumber;
	}

}