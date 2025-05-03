#include "ComplexPlane.h"
#include "SFML/Graphics.hpp"

using namespace sf;
using namespace std;

int main()
{
	int pixelWidth = VideoMode::getDesktopMode().width / 2;

	int pixelHeight = VideoMode::getDesktopMode().height / 2;

	VideoMode vm(pixelWidth, pixelHeight);
	RenderWindow window(vm, "Mandelbrot", Style::Default);
	ComplexPlane complexPlane(pixelWidth, pixelHeight);
	
	Font font;
	
	if (!font.loadFromFile("arial.ttf")) 
	{
		return -1;
	}
	Text text;
	text.setFont(font);
	text.setCharacterSize(15);
	text.setFillColor(Color::White);
	text.setPosition(10, 10);
	
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}
			if (event.type == Event::MouseButtonPressed)
			{
				//zoom in for right click
				if (event.mouseButton.button == Mouse::Right)
				{
					Vector2i center = { event.mouseButton.x, event.mouseButton.y };
					complexPlane.zoomIn();
					complexPlane.setCenter(center);
				}
				//zoom out for left click
				if (event.mouseButton.button == Mouse::Left)
				{
					Vector2i center = { event.mouseButton.x, event.mouseButton.y };
					complexPlane.zoomOut();
					complexPlane.setCenter(center);
				}
			}
			//handle moving mouse
			if (event.type == Event::MouseMoved)
			{
				Vector2i mouseLocation = Mouse::getPosition(window);
				complexPlane.setMouseLocation(mouseLocation);
			}
			if (Keyboard::isKeyPressed(Keyboard::Escape))
			{
				window.close();
			}
		}
		complexPlane.updateRender();
		complexPlane.loadText(text);
		window.clear(Color::Black);
		complexPlane.draw(window, RenderStates::Default);
		window.draw(text);

		//window.draw(...);

		window.display();
	}
}
