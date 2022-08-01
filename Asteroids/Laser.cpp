#include "Laser.h"
#include "Renderer.h"
#include "Asteroid.h"
#include "Inih/cpp/INIReader.h"
#include "EnemyShip.h"

#include <cmath>

const float Pi = (float)3.141592;
const float Rad = Pi / 180;

Laser::Laser(Renderer* _Rend)
	:Active(false),
	Rend(_Rend),
	DeltaTime(0),
	TimeRemaining(0)
{
	INIReader ConFile("InitialData.ini");

	if (ConFile.ParseError() < 0)
		ConFile.PrintError("Laser");

	Width = ConFile.GetInteger("Laser", "Width", 0);
	Height = ConFile.GetInteger("Laser", "Height", 0);
	Velocity = ConFile.GetInteger("Laser", "Velocity", 0);
	LifeTime = float(ConFile.GetInteger("Laser", "LifeTime", 0));
	FirstPoint.x = SecondPoint.x = ThirdPoint.x = FourthPoint.x = Center.x = -100;
	FirstPoint.y = SecondPoint.y = ThirdPoint.y = FourthPoint.y = Center.y = -100;
	FirstPoint.Rotation = SecondPoint.Rotation = ThirdPoint.Rotation = FourthPoint.Rotation = Center.Rotation = 0;
	FirstPoint.Angle = SecondPoint.Angle = ThirdPoint.Angle = FourthPoint.Angle = Center.Angle = 0;
	H = (float)sqrt(pow(Height, 2) + pow(Width / 2, 2)); // NOTE(isaveg): you are still using the wrong math functions for float values

	P1.x = P2.x = P3.x = P4.x = 0;
	P1.y = P2.y = P3.y = P4.y = 0;
	
}

Laser::~Laser()
{
}

void Laser::Update(bool Pause)
{
	if (!Pause)
	{
		DeltaTime = Rend->getDeltaTime();
		Center.x += cos(Center.Rotation) * Velocity * DeltaTime; // NOTE(isaveg): you are still using the wrong math functions for float values
		Center.y -= sin(Center.Rotation) * Velocity * DeltaTime; // NOTE(isaveg): you are still using the wrong math functions for float values
		TimeRemaining -= DeltaTime;

		if (Center.x > Rend->getWindowWidth())
			Center.x = 0;
		else if (Center.x < 0)
			Center.x = (float) Rend->getWindowWidth();

		if (Center.y > Rend->getWindowHeight())
			Center.y = 0;
		else if (Center.y < 0)
			Center.y = (float)Rend->getWindowHeight();

		FirstPoint.x = Center.x + P1.x;
		FirstPoint.y = Center.y + P1.y;
		SecondPoint.x = Center.x + P2.x;
		SecondPoint.y = Center.y + P2.y;
		ThirdPoint.x = Center.x + P3.x;
		ThirdPoint.y = Center.y + P3.y;
		FourthPoint.x = Center.x + P4.x;
		FourthPoint.y = Center.y + P4.y;
	}

	Rend->DrawRect(&FirstPoint, &SecondPoint, &ThirdPoint, &FourthPoint, 255, 255, 255, 255);

	if (TimeRemaining <= 0)
		Active = false;

}

void Laser::setPosition(float x, float y, int _Angle, float _Rotation)
{
	Center.x = x;
	Center.y = y;
	Center.Angle = _Angle;
	Center.Rotation = _Rotation;

	FirstPoint.Angle = Center.Angle + 315;
	SecondPoint.Angle = Center.Angle + 45;
	ThirdPoint.Angle = Center.Angle + 135;
	FourthPoint.Angle = Center.Angle + 225;

	FirstPoint.Rotation = FirstPoint.Angle * Rad;
	SecondPoint.Rotation = SecondPoint.Angle * Rad;
	ThirdPoint.Rotation = ThirdPoint.Angle * Rad;
	FourthPoint.Rotation = FourthPoint.Angle * Rad;

	// NOTE(isaveg): you are still using the wrong math functions for float values
	P1.x = cos(FirstPoint.Rotation) * H;
	P1.y = -sin(FirstPoint.Rotation) * H;
	P2.x = cos(SecondPoint.Rotation) * H;
	P2.y = -sin(SecondPoint.Rotation) * H;
	P3.x = cos(ThirdPoint.Rotation) * H;
	P3.y = -sin(ThirdPoint.Rotation) * H;
	P4.x = cos(FourthPoint.Rotation) * H;
	P4.y = -sin(FourthPoint.Rotation) * H;

	TimeRemaining = LifeTime;
}

bool Laser::CheckCollision(Position* Pos, double ObjectH)
{
	if (sqrt(pow(Pos->x - Center.x, 2) + pow(Pos->y - Center.y, 2)) < H + ObjectH) // NOTE(isaveg): you are still using the wrong math functions for float values
	{
		Active = false;
		return true;
	}

	return false;
}

