## Introduction course to 3D-programming with DirectX11 and C++

# Lab 1
* Make a raytracer that could export an image of a scene of object
* Sending rays from the "camera" to the objects (plane, sphere, triangle and oriented boundingbox)
* stb_image_write.h was used as an extern library to write the final image

![lab1.png](https://github.com/FilipWickstrom/Learning_D3D11/blob/main/Images/lab1.png)

# Lab 2
* Make a basic 3D-renderer that could render a moving quad
* Learned the basic pipeline
* stb_image.h was used as an extern library to read the textures

![lab2.gif](https://github.com/FilipWickstrom/Learning_D3D11/blob/main/Images/lab2.gif)

# The final project
This project was about making our own 3D-renderer from scratch where we picked different techniques to learn. I picked:
* Deferred rendering
* Model format parsing with .obj and .mtl
* Displacement mapping with tessellation hardware
* Normal mapping
* Shadow mapping
* Back face culling with geometry shader
* Gaussian filter in compute shader
* Bilateral filter in compute shader
* Running water effect - CPU-based, which is probably not the best

[![TheFinalProject.png](https://github.com/FilipWickstrom/Learning_D3D11/blob/main/Images/TheFinalProject.png)](https://www.youtube.com/watch?v=8EDrdRsyVGs)
* Click the image to see a video of it. An update version is coming up later


Credits:
* Fever Dream: https://steamcommunity.com/sharedfiles/filedetails/?id=605832569
* Water: https://3dtextures.me/2017/12/28/water-001/
* Floor: https://3dtextures.me/2021/01/12/stylized-stone-wall-001/
* Wet rocks: https://3dtextures.me/2020/10/30/ground-wet-rocks-002/
* Brick wall: https://3dtextures.me/2019/11/21/brick-wall-017/
* Brown brick wall: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
* Image loading with STB: https://github.com/nothings/stb
* Keyboard and mouse: https://github.com/microsoft/DirectXTK

# How to build and test?
There is a solution for every project and they can all be built with premake. By default it uses Visual Studio 2019 but you can change it in the "build.bat" file if you need anything else. The projects was programmed on Visual Studio 2019 was made for Windows 10 64-bit.

There is also some zipped files with the final results. They are named "EXE.zip" and can be runned by unzipping and clicking on the executable file :)

# Warning
Most the code is a bit messy and not the best structured and optimized as it was my first time learning about using the D3D11 API and 3D-rendering in general.
