#include "Platform/Platform.hpp"
#include "movementVector.hpp"
#include "obstacles.hpp"
#include <math.h>
#include <vector>

using namespace std;
using namespace sf;

float getIntersection(float Ax1, float Ay1, float Ax2, float Ay2, float Bx1, float By1, float Bx2, float By2);
float getIntersection(float Ax1, float Ay1, float Ax2, float Ay2, float Bx1, float By1, float Bx2, float By2)
{
	float denominatorM1 = (Ax1 - Ax2);
	float denominatorM2 = (Bx1 - Bx2);
	float m1 = (Ay1 - Ay2) / denominatorM1;
	float m2 = (By1 - By2) / denominatorM2;
	float c1 = Ay1 - m1 * Ax1;
	float c2 = By1 - m2 * Bx1;

	if (!denominatorM1)
	{
		cout << "IntXAx1: " << Ax1 << "\n";
		return Ax1;
	}
	if (!denominatorM2)
	{
		cout << "IntXBx1: " << Bx1 << "\n";
		return Bx1;
	}

	float denominator = m2 - m1;

	float xIntersection = (c1 - c2) / denominator;
	return xIntersection;
}

float getYFromX(float xVal, float Ax1, float Ay1, float Ax2, float Ay2);
float getYFromX(float xVal, float Ax1, float Ay1, float Ax2, float Ay2)
{
	float m1 = (Ay1 - Ay2) / (Ax1 - Ax2);
	float c1 = Ay1 - m1 * Ax1;

	float yIntersection = m1 * xVal + c1;
	return yIntersection;
}

bool sortByAngle(movementVector& v1, movementVector& v2);
bool sortByAngle(movementVector& v1, movementVector& v2)
{
	return v1.getDirection() < v2.getDirection();
}

int main()
{
	util::Platform platform;

#if defined(_DEBUG)
	std::cout << "Hello World!" << std::endl;
#endif
	movementVector light;
	vector<lineObj> obstacles;

	float mouseBtn[2];
	float mouseCoord[2];
	float savedMouseCoord[2] = { 0, 0 };
	bool saved = false;
	bool mouseDown = false;
	bool drawPoint = false;
	int currentId = 0;

	sf::RenderWindow window;
	// in Windows at least, this must be called before creating the window
	float screenScalingFactor = platform.getScreenScalingFactor(window.getSystemHandle());
	// Use the screenScalingFactor
	window.create(sf::VideoMode(1000.0f * screenScalingFactor, 1000.0f * screenScalingFactor), "Ray Cast!");
	platform.setIcon(window.getSystemHandle());

	sf::CircleShape pointDraw(4);
	pointDraw.setFillColor(sf::Color::Blue);

	sf::VertexArray triangle(sf::Triangles, 3);

	// Limit the framerate to 60 frames per second (this step is optional)
	window.setFramerateLimit(60);

	// Game draw objects
	sf::Font font;
	if (!font.loadFromFile("/home/oggyp/Projects/C++/Boids/lib/Arial.ttf"))
	{
		cout << "Error loading font";
	}
	sf::Text directionText;
	directionText.setFillColor(sf::Color::White);
	directionText.setCharacterSize(24);
	directionText.setFont(font);
	directionText.setPosition(100, 100);
	sf::VertexArray vectorDraw(sf::LinesStrip, 2);

	sf::Event event;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			// catch the resize events
			if (event.type == sf::Event::Resized)
			{
				// update the view to the new size of the window
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}
		}

		mouseBtn[0] = sf::Mouse::isButtonPressed(sf::Mouse::Left);
		mouseBtn[1] = sf::Mouse::isButtonPressed(sf::Mouse::Right);
		mouseCoord[0] = sf::Mouse::getPosition(window).x;
		mouseCoord[1] = sf::Mouse::getPosition(window).y;

		if (mouseBtn[0] && !mouseDown)
		{
			mouseDown = true;
			if (!saved)
			{
				saved = true;
				drawPoint = true;
				savedMouseCoord[0] = mouseCoord[0];
				savedMouseCoord[1] = mouseCoord[1];
			}
			else
			{
				saved = false;
				drawPoint = false;
				lineObj newLine(currentId);
				currentId++;
				newLine.x1 = savedMouseCoord[0];
				newLine.y1 = savedMouseCoord[1];
				newLine.x2 = mouseCoord[0];
				newLine.y2 = mouseCoord[1];
				obstacles.push_back(newLine);
			}
		}
		else if (!mouseBtn[0])
		{
			mouseDown = false;
		}

		window.clear();

		vectorDraw[0].color = sf::Color::White;
		vectorDraw[1].color = sf::Color::White;

		vector<movementVector> points;

		for (auto currentLine : obstacles)
		{
			for (int pointNum = 1; pointNum < 3; pointNum++)
			{
				movementVector mv;
				// double obstaclesCollided[3] = { 1000000000000, 0, 0 };
				mv.x = currentLine.getX(pointNum) - mouseCoord[0];
				mv.y = currentLine.getY(pointNum) - mouseCoord[1];
				points.push_back(mv);
				// for (auto checkLine : obstacles)
				// {
				// 	if (checkLine.id != currentLine.id)
				// 	{
				// 		float intersectionX = getIntersection(checkLine.x1, checkLine.y1, checkLine.x2, checkLine.y2, coord1[0], coord1[1], coord2[0], coord2[1]);

				// 		if (((intersectionX < checkLine.x2 && intersectionX > checkLine.x1) || (intersectionX > checkLine.x2 && intersectionX < checkLine.x1)) && ((mv.x > coord1[0] && intersectionX > coord1[0]) || (mv.x < coord1[0] && intersectionX < coord1[0])))
				// 		{
				// 			float intersectionY = getYFromX(intersectionX, checkLine.x1, checkLine.y1, checkLine.x2, checkLine.y2);
				// 			float distance = (mouseCoord[0] - intersectionX) * (mouseCoord[0] - intersectionX) + (mouseCoord[1] - intersectionY) * (mouseCoord[1] - intersectionY);
				// 			if (distance < obstaclesCollided[0])
				// 			{
				// 				obstaclesCollided[0] = distance;
				// 				obstaclesCollided[1] = intersectionX;
				// 				obstaclesCollided[2] = intersectionY;
				// 			}
				// 		}
				// 	}
				// }
				// if (obstaclesCollided[0] < 1000000000000)
				// {
				// 	coord2[0] = obstaclesCollided[1];
				// 	coord2[1] = obstaclesCollided[2];
				// }
				// vectorDraw[0].position = sf::Vector2f(coord1[0], coord1[1]);
				// vectorDraw[1].position = sf::Vector2f(coord2[0], coord2[1]);
				// window.draw(vectorDraw);
			}
		}

		double direction = 10000000000;

		for (int parses = 0; parses < 2; parses++)
		{
			movementVector newMv1;
			newMv1.x = direction;
			newMv1.y = 0;
			points.push_back(newMv1);
			newMv1.y = direction;
			newMv1.x = 0;
			points.push_back(newMv1);
			direction = -10000000000;
		}

		int m = points.size();

		sort(points.begin(), points.end(), sortByAngle);

		for (int i = 0; i < m; i++)
		{
			movementVector point = points[i];
			double originalMagSqr = point.getMagnitudeSqr();
			double obstaclesCollided[3] = { originalMagSqr, 0, 0 };
			double finalCoord[2] = { point.x + mouseCoord[0], point.y + mouseCoord[1] };
			for (auto checkLine : obstacles)
			{
				float intersectionX = getIntersection(checkLine.x1, checkLine.y1, checkLine.x2, checkLine.y2, mouseCoord[0], mouseCoord[1], finalCoord[0], finalCoord[1]);
				float intersectionY = getYFromX(intersectionX, checkLine.x1, checkLine.y1, checkLine.x2, checkLine.y2);
				if (((intersectionX < checkLine.x2 && intersectionX > checkLine.x1) || (intersectionX > checkLine.x2 && intersectionX < checkLine.x1))
					&& (((finalCoord[0] > mouseCoord[0] && intersectionX > mouseCoord[0]) || (finalCoord[0] < mouseCoord[0] && intersectionX < mouseCoord[0]))
						|| ((finalCoord[1] > mouseCoord[1] && intersectionY > mouseCoord[1]) || (finalCoord[1] < mouseCoord[1] && intersectionY < mouseCoord[1]))))
				{
					float distance = (mouseCoord[0] - intersectionX) * (mouseCoord[0] - intersectionX) + (mouseCoord[1] - intersectionY) * (mouseCoord[1] - intersectionY);
					if (distance < obstaclesCollided[0])
					{
						obstaclesCollided[0] = distance;
						obstaclesCollided[1] = intersectionX;
						obstaclesCollided[2] = intersectionY;
					}
				}
			}
			if (obstaclesCollided[0] < originalMagSqr)
			{
				finalCoord[0] = obstaclesCollided[1];
				finalCoord[1] = obstaclesCollided[2];
			}
			vectorDraw[0].position = sf::Vector2f(mouseCoord[0], mouseCoord[1]);
			vectorDraw[1].position = sf::Vector2f(finalCoord[0], finalCoord[1]);
			window.draw(vectorDraw);
		}

		vectorDraw[0].color = sf::Color::Blue;
		vectorDraw[1].color = sf::Color::Blue;

		for (auto currentLine : obstacles)
		{
			vectorDraw[0].position = sf::Vector2f(currentLine.x1, currentLine.y1);
			vectorDraw[1].position = sf::Vector2f(currentLine.x2, currentLine.y2);
			window.draw(vectorDraw);
		}

		if (drawPoint)
		{
			pointDraw.setPosition(savedMouseCoord[0], savedMouseCoord[1]);
			window.draw(pointDraw);
		}

		window.display();
	}

	return 0;
}
