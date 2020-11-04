#include <iostream>
#include <stdint.h>	//For uint8_t
#include <vector>

#include "Plane.h"
#include "Sphere.h"
#include "OrientedBoundingBox.h"

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
	//Plane plane(Vector3D(0, 255, 0), Vector3D(0,0,-1), Vector3D(0,0,100));
	//shapes.push_back(std::make_unique<Plane>(plane));
	//Sphere sphere(Vector3D(255, 0, 0), Vector3D(width/2, height/2, 50), 50);
	//shapes.push_back(std::make_unique<Sphere>(sphere));
	OrientedBoundBox obb(Vector3D(255, 20, 147), Vector3D(width*3/4,height*3/4,150), Vector3D(25,0,0), Vector3D(0,25,0), Vector3D(0,0,25));//Kubform just nu. Ändra senare
	shapes.push_back(std::make_unique<OrientedBoundBox>(obb));

	//Do the raycasting
	for (unsigned int h = 0; h < height; ++h)
	{
		for (unsigned int w = 0;  w < width;  ++w)
		{
			Ray ray({ static_cast<double>(w), static_cast<double>(h), 0 }, { 0,0,1 });
			int currentPixel = (h * width + w) * channels;
			double currentClosest = std::numeric_limits<double>::max();

			// Default colour
			imageData[currentPixel + 0] = 255;
			imageData[currentPixel + 1] = 255;
			imageData[currentPixel + 2] = 255;

			for (auto& shape : shapes)
			{
				double t = 0;
				if (shape->Intersection(ray, t) && t < currentClosest)
				{
					const Vector3D& colour = shape->GetColour();
					imageData[currentPixel + 0] = static_cast<uint8_t>(colour.GetX());
					imageData[currentPixel + 1] = static_cast<uint8_t>(colour.GetY());
					imageData[currentPixel + 2] = static_cast<uint8_t>(colour.GetZ());
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

	//Testing 
	//Vector3D test1337[3];
	//test1337[0] = Vector3D(1337, 1337, 1337);
	//Vector3D testvector[3];
	//testvector[0] = test1337[0];
	////testvector[1] = Vector3D(200, 200, 200);
	//std::cout << "\nVec[0].GetX(): " << testvector[0].GetX() << std::endl;
	//testvector[0].Normalize();
	//std::cout << "Vec[0].GetX(): " << testvector[0].GetX() << std::endl;

	return 0;
}