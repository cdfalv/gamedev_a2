#include "Vec2.h"
#include <math.h>
#include <ostream>

Vec2::Vec2()
{

}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	//TODO
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	//TODO
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator * (const float val) const
{
	//TODO
	return Vec2(x * val, y * val);
}

Vec2 Vec2::operator / (const float val) const
{
	//TODO
	return Vec2(x / val, y / val);
}

bool Vec2::operator == (const Vec2& rhs) const
{
	//TODO
	if (x == rhs.x && y == rhs.y)
		return true;
	else
		return false;
}

bool Vec2::operator != (const Vec2& rhs) const
{
	//TODO
	if (x != rhs.x || y != rhs.y)
		return true;
	else
		return false;
}

void Vec2::operator += (const Vec2& rhs)
{
	//TODO
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	//TODO
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator *= (const float val)
{
	//TODO
	x *= val;
	y *= val;
}

void Vec2::operator /= (const float val)
{
	//TODO
	x /= val;
	y /= val;
}

float Vec2::dist(const Vec2& rhs) const
{
	//TODO

	return 0;
}

float Vec2::length() const
{
	return sqrtf(x*x + y*y);
}

Vec2 Vec2::normalize() const
{
	float l = length();
	return Vec2(x / l , y/ l);
}
