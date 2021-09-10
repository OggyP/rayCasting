class lineObj
{
public:
	double x1 = 0;
	double y1 = 0;
	double x2 = 0;
	double y2 = 0;
	int id = 0;
	lineObj(int lineId)
	{
		id = lineId;
	}
	double getX(int pointNum)
	{
		if (pointNum == 1)
		{
			return x1;
		}
		else
		{
			return x2;
		}
	}
	double getY(int pointNum)
	{
		if (pointNum == 1)
		{
			return y1;
		}
		else
		{
			return y2;
		}
	}
};