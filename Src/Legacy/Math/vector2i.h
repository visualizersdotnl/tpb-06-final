#pragma once



class Vector2i
{
public:

	Vector2i()
		:x(0),y(0)
	{}

	Vector2i(int x, int y)
		:x(x),y(y)
	{}

	Vector2i(const Vector2i& b)
		:x(b.x),y(b.y)
	{}

	void operator +=(const Vector2i& other)
	{
		x+=other.x;
		y+=other.y;
	}

	void operator -=(const Vector2i& other)
	{
		x-=other.x;
		y-=other.y;
	}

	Vector2i operator-(const Vector2i& other) const
	{
		return Vector2i(x-other.x,y-other.y);
	}

	Vector2i operator+(const Vector2i& other) const
	{		
		return Vector2i(x+other.x,y+other.y);
	}

	Vector2i operator/(int scalar) const
	{
		return Vector2i((int)(x/scalar), (int)(y/scalar));
	}

	Vector2i operator-() const
	{
		return Vector2i(-x, -y);
	}

	bool operator==(const Vector2i& other) const
	{
		return (other.x == x) && (other.y == y);
	}

	int x, y;
};