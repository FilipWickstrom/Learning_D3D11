## This was an introduction course to learning 3D-programming with DirectX 11 and C++

# Lab 1
* Make a raytracer that could export an image of a scene of object
* Sending rays from the "camera" to the objects (plane, sphere, triangle and oriented boundingbox)
* stb_image_write.h was used as an extern library to write the final image

**Final image**

![lab1.png](https://github.com/FilipWickstrom/Learning_D3D11/blob/main/Lab1/Lab1_RayTracing/Lab1_RayTracing/lab1.png)

# Lab 2
* Make a basic 3D-renderer that could render a moving quad
* Learned the basic pipeline
* stb_image.h was used as an extern library to read the textures

**Final 3D renderer**

![lab2.png](https://github.com/FilipWickstrom/Learning_D3D11/blob/main/Lab2/Lab2_HelloTriangle/TechFlipLogoRotation.gif)

# The final project
This project was about making our own 3D-renderer from scratch where we picked different techniques to learn. I picked:
  1. Deferred rendering
  2. Model format parsing with .obj and .mtl
  3. Displacement mapping with tessellation hardware
  4. Normal mapping
  5. Shadow mapping
  6. Back face culling with geometry shader
  7. Gaussian filter in compute shader
  8. Bilateral filter in compute shader
  9. Running water effect - CPU-based, which is probably not the best

**ERROR 404 - Video not found... coming soon**

Credits:
* Fever Dream: https://steamcommunity.com/sharedfiles/filedetails/?id=605832569
* Water: https://3dtextures.me/2017/12/28/water-001/
* Floor: https://3dtextures.me/2021/01/12/stylized-stone-wall-001/
* Wet rocks: https://3dtextures.me/2020/10/30/ground-wet-rocks-002/
* Brick wall: https://3dtextures.me/2019/11/21/brick-wall-017/
* Brown brick wall: https://learnopengl.com/Advanced-Lighting/Normal-Mapping
* Image loading with STB: https://github.com/nothings/stb
* Keyboard and mouse: https://github.com/microsoft/DirectXTK
