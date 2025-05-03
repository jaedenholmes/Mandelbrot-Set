#include "ComplexPlane.h"

int ComplexPlane::countIterations(Vector2f coord)
{
	float zx = 0.0f;
	float zy = 0.0f;
	int iter = 0;

	while (zx * zx + zy * zy <= 4.0f && iter < MAX_ITER) {
		float temp = zx * zx - zy * zy + coord.x;
		zy = 2.0f * zx * zy + coord.y;
		zx = temp;
		++iter;
	}

	return iter;
}

void ComplexPlane::iterationsToRGB(size_t count, Uint8& r, Uint8& g, Uint8& b)
{
	//divide iter into 5 sections, 0-51, 51-102, 103-153, 153-204, 204-255
	//normalize to [0,1)
	float t = static_cast<float>(count) / MAX_ITER;
	//now the fice sections are 0 - 0.2, 0.2 - 0.4, 0.4 - 0.6, 0.6 - 0.8, and 0.8 to 1
	
	//if iter reaches 64 then set color to black
	if (count >= MAX_ITER)
	{
		r = 0;
		g = 0;
		b = 0;
		return;
	}
	else
	{
		///bright yellow sliding effect, more iterations = brighter, fuller and less = darker, duller
		t = sqrt(t);  // smoother curve
		r = static_cast<Uint8>(t * 255); // increase red
		g = static_cast<Uint8>(t * 255); // increase green
		b = 0;
	}
	
	
	/*
	if (t > 0.2)
	{
		//purple to blue
		r = static_cast<Uint8>(128 - 128 * t / 0.2f);  // decrease red so less purple
		g = 0;
		b = static_cast<Uint8>(255 * t / 0.2f);        // increase blue
	}
	else if (t < 0.4)
	{
		//blue to turquoise
		r = 0;
		g = static_cast<Uint8>(255 * (t - 0.2f) / 0.2f);	//increase green
		b = 255;											//max blue
	}
	else if (t < 0.6)
	{
		//turqouise to green
		r = 0;
		g = 255;												//max green
		b = static_cast<Uint8>(255 - 255 * (t - 0.4f) / 0.2f);	//decrease blue'
	}
	else if (t < 0.8)
	{
		//green to yellow
		r = static_cast<Uint8>(255 * (t - 0.8f));		//increase red
		g = 255;
		b = 0;
	}
	else
	{
		//yellow to red
		r = 255;
		g = static_cast<Uint8>(255 - 255 * (t - 0.4f) / 0.2f);
		b = 0;
		
	}*/
}

Vector2f ComplexPlane::mapPixelToCoords(Vector2i mousePixel)
{
	//formula to map one range into another: ((n  - a) / (b - a)) * (d - c) + c
	//
	float real = (mousePixel.x / (m_pixel_size.x - 0.0f)) * m_plane_size.x + (m_plane_center.x - m_plane_size.x / 2.0);

	float imag = ((m_pixel_size.y - mousePixel.y) / (m_pixel_size.y - 0.0f)) * m_plane_size.y + (m_plane_center.y - m_plane_size.y / 2.0);

	return Vector2f(real, imag);
}

ComplexPlane::ComplexPlane(int pixelWidth, int pixelHeight)
{
	m_pixel_size.x = pixelWidth;
	m_pixel_size.y = pixelHeight;
	m_aspectRatio = static_cast<float>(pixelHeight) / static_cast<float>(pixelWidth);
	m_plane_center = {0 , 0};
	
	//Base width and height 4.0, 
	m_plane_size = { BASE_WIDTH, BASE_HEIGHT * m_aspectRatio};
	m_zoomCount = 0;
	m_state = State::CALCULATING;
	m_vArray.setPrimitiveType(sf::Points);
	m_vArray.resize(m_pixel_size.x * m_pixel_size.y);
}

void ComplexPlane::draw(RenderTarget& target, RenderStates states) const
{
	target.draw(m_vArray);
}

void ComplexPlane::zoomIn()
{
	m_zoomCount++;
	float xZoom = BASE_WIDTH * (pow(BASE_ZOOM, m_zoomCount));
	float yZoom = BASE_HEIGHT * m_aspectRatio * (pow(BASE_ZOOM, m_zoomCount));

	m_plane_size = { xZoom, yZoom };
	m_state = State::CALCULATING;
}

void ComplexPlane::zoomOut()
{
	m_zoomCount--;
	float xZoom = BASE_WIDTH * (pow(BASE_ZOOM, m_zoomCount));
    float yZoom = BASE_HEIGHT * m_aspectRatio * (pow(BASE_ZOOM, m_zoomCount));

	m_plane_size = { xZoom, yZoom };
	m_state = State::CALCULATING;
}

void ComplexPlane::setCenter(Vector2i mousePixel)
{
	m_plane_center = mapPixelToCoords(mousePixel);
	m_state = State::CALCULATING;
}

void ComplexPlane::setMouseLocation(Vector2i mousePixel)
{
	m_mouseLocation = mapPixelToCoords(mousePixel);
}

void ComplexPlane::loadText(Text& text)
{
	stringstream ss;

	ss << "Mandelbrot Set" << endl;
	ss << "Center: (" << m_plane_center.x << "," << m_plane_center.y << ")" << endl;
	ss << "Cursor: (" << m_mouseLocation.x << "," << m_mouseLocation.y << ")" << endl;
	ss << "Left-click to zoom in" << endl;
	ss << "Right-click to zoom out" << endl;

	text.setString(ss.str());
}

void ComplexPlane::updateRender()
{
	if (m_state == State::CALCULATING)
	{
		std::cout << "Pixel size: (" << m_pixel_size.x << ", " << m_pixel_size.y << ")" << std::endl;
		for (int j = 0; j < m_pixel_size.x; j++)
			for (int i = 0; i < m_pixel_size.y; i++)
			{
				int index = j + i * m_pixel_size.x;
				
				m_vArray[index].position = {static_cast<float>(j), static_cast<float>(i)};
				
				Vector2f newCoord = mapPixelToCoords({ j, i });
				int coordIter = countIterations(newCoord);
				
				Uint8 r, g, b;
				iterationsToRGB(coordIter, r, g, b);
				m_vArray[index].color = { r,g,b };
			}
	}
	m_state = State::DISPLAYING;
}
