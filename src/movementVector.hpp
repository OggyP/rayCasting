#include <math.h>

const double PIlib = 3.141592653589793238463;

class movementVector
{
public:
	double x = 0;
	double y = 0;

	int id = 0;

	double getMagnitude()
	{
		const double magnitudeFromVector = sqrt(pow(x, 2) + pow(y, 2));
		return magnitudeFromVector;
	}

	double getMagnitudeSqr()
	{
		return x * x + y * y;
	}

	double getDirection()
	{
		double direction = atan2(y, x);
		return direction;
	}

	void setVector(double direction, double magnitude)
	{
		x = magnitude * cos(direction);
		y = magnitude * sin(direction);
	}
};