#include "Platform/Platform.hpp"
#include "movementVector.hpp"
#include "obstacles.hpp"
#include <math.h>
#include <pthread.h>
#include <vector>

using namespace std;
using namespace sf;

float getIntersection(float Ax1, float Ay1, float Ax2, float Ay2, float Bx1, float By1, float Bx2, float By2);
float getIntersection(float Ax1, float Ay1, float Ax2, float Ay2, float Bx1, float By1, float Bx2, float By2)
{
	float m1 = (Ay1 - Ay2) / (Ax1 - Ax2);
	float m2 = (By1 - By2) / (Bx1 - Bx2);
	float c1 = Ay1 - m1 * Ax1;
	float c2 = By1 - m2 * Bx1;

	float xIntersection = (c1 - c2) / (m2 - m1);
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

	sf::RenderWindow window;
	// in Windows at least, this must be called before creating the window
	float screenScalingFactor = platform.getScreenScalingFactor(window.getSystemHandle());
	// Use the screenScalingFactor
	window.create(sf::VideoMode(1000.0f * screenScalingFactor, 1000.0f * screenScalingFactor), "Ray Cast!");
	platform.setIcon(window.getSystemHandle());

	sf::CircleShape pointDraw(4);
	pointDraw.setFillColor(sf::Color::Blue);

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
				lineObj newLine;
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

		for (float angle = -M_PI; angle <= M_PI; angle += M_PI / 9999)
		{
			double coord1[2] = { mouseCoord[0], mouseCoord[1] };
			movementVector mv;
			mv.setVector(angle, 1000000);
			mv.x += mouseCoord[0];
			mv.y += mouseCoord[1];
			double coord2[2] = { mv.x, mv.y };
			double obstaclesCollided[3] = { 1000000000000, 0, 0 };
			for (auto currentLine : obstacles)
			{
				float intersectionX = getIntersection(currentLine.x1, currentLine.y1, currentLine.x2, currentLine.y2, coord1[0], coord1[1], coord2[0], coord2[1]);

				if (((intersectionX < currentLine.x2 && intersectionX > currentLine.x1) || (intersectionX > currentLine.x2 && intersectionX < currentLine.x1)) && ((mv.x > coord1[0] && intersectionX > coord1[0]) || (mv.x < coord1[0] && intersectionX < coord1[0])))
				{
					float intersectionY = getYFromX(intersectionX, currentLine.x1, currentLine.y1, currentLine.x2, currentLine.y2);
					float distance = (mouseCoord[0] - intersectionX) * (mouseCoord[0] - intersectionX) + (mouseCoord[1] - intersectionY) * (mouseCoord[1] - intersectionY);
					if (distance < obstaclesCollided[0])
					{
						obstaclesCollided[0] = distance;
						obstaclesCollided[1] = intersectionX;
						obstaclesCollided[2] = intersectionY;
					}
				}
			}
			if (obstaclesCollided[0] < 1000000000000)
			{
				coord2[0] = obstaclesCollided[1];
				coord2[1] = obstaclesCollided[2];
			}
			vectorDraw[0].position = sf::Vector2f(coord1[0], coord1[1]);
			vectorDraw[1].position = sf::Vector2f(coord2[0], coord2[1]);
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
