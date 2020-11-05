#include <iostream>
#include <stdint.h>	//For uint8_t
#include <vector>

#include "Plane.h"
#include "Sphere.h"
#include "OrientedBoundingBox.h"
#include "Triangle.h"

//For creating a image
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main()
{
	const unsigned int width = 512;
	const unsigned int height = 512;
	const int channels = 3;	//rgb
	uint8_t* imageData = new uint8_t[width * height * channels];
	std::vector<std::unique_ptr<Shape>> shapes;

	//Creates all objects in the scene
	Plane plane(Vector3D(0, 255, 0), Vector3D(0,0,-1), Vector3D(0,0,150));
	shapes.push_back(std::make_unique<Plane>(plane));
	
	Sphere sphere(Vector3D(255, 0, 0), Vector3D(width/3, height/2, 75), 75);
	shapes.push_back(std::make_unique<Sphere>(sphere));
	
	OrientedBoundBox obb(Vector3D(255, 111, 255), Vector3D(width*3/4,height*3/4,75), Vector3D(50,0,-25), Vector3D(-25,50,0), Vector3D(0,0,50));		//FIX LATER****
	shapes.push_back(std::make_unique<OrientedBoundBox>(obb));

	Triangle triangle(Vector3D(0, 0, 255), Vector3D(width/3, 5, 100), Vector3D(width/4, 400, 5), Vector3D(width/2, 450, 5));
	shapes.push_back(std::make_unique<Triangle>(triangle));

	//Light
	const double lightRange = 180;

	//Do the raycasting
	for (unsigned int h = 0; h < height; ++h)
	{
		for (unsigned int w = 0;  w < width;  ++w)
		{
			Ray ray({ static_cast<double>(w), static_cast<double>(h), 0 }, { 0,0,1 });
			int currentPixel = (h * width + w) * channels;
			double currentClosest = std::numeric_limits<double>::max();

			//Default colour
			imageData[currentPixel + 0] = 255;
			imageData[currentPixel + 1] = 255;
			imageData[currentPixel + 2] = 255;

			for (auto& shape : shapes)
			{
				double t = 0;
				if (shape->Intersection(ray, t) && t < currentClosest)
				{
					//Shading
					double distansToIntersection = t;
					double lightFactor = 1 - (distansToIntersection / lightRange);
					if (lightFactor > 1)
						lightFactor = 1;
					else if (lightFactor < 0)
						lightFactor = 0;

					const Vector3D& colour = shape->GetColour();
					imageData[currentPixel + 0] = static_cast<uint8_t>(colour.GetX() * lightFactor);
					imageData[currentPixel + 1] = static_cast<uint8_t>(colour.GetY() * lightFactor);
					imageData[currentPixel + 2] = static_cast<uint8_t>(colour.GetZ() * lightFactor);
					currentClosest = t;
				}
			}
		}
	}
		
	//Creates the image
	if (stbi_write_png("lab1.png", width, height, channels, imageData, width * channels) != 1)
	{
		std::cout << "ERROR: could not create the picture... stbi_write_png failed" << std::endl;
	}

	return 0;
}