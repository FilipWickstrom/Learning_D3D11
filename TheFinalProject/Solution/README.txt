Made by Filip Wickström - https://github.com/FilipWickstrom

-----Techniques implemented-----
2.1 Core Techniques:
 * 2.1.1 Deferred rendering

2.2 Geometry:
 * 2.2.1 Model Format Parsing
 * 2.2.2 Displacement Mapping using Tessellation Hardware

2.3 Texturing and Lighting:
 * 2.3.1 Normal Mapping

2.4 Projection Techniques:
 * 2.4.1 Shadow Mapping

2.5 Acceleration Techniques:
 * 2.5.3 Back Face Culling using Geometry Shader 

2.6 Other Techniques:
 * 2.6.2 Gaussian Filter in Compute Shader
 * 2.6.3 Bilateral Filter in Compute Shader
 * 2.6.6 Water-effect (CPU-based)

-----Controls-----
Movement:
 * Forward: 		W
 * Backward: 		S
 * Left: 		A
 * Right: 		D
 * Up:			Space
 * Down:		Left Control
 * Camera movement: 	Mouse2 is held down

Tessellation
Number of triangles created when pressed:
 * 1.0f:		Key 1
 * 4.0f:		Key 2
 * 8.0f:		Key 3
 * 16.0f:  		Key 4
 * 32.0f:		Key 5
Percent of displacement map:
 * 0.2f:		Key 6
 * 0.4f:		Key 7
 * 0.6f:		Key 8
 * 0.8f:		Key 9
 * 1.0f:		Key 0

Wireframe
 * ON/OFF: 		V or B

Normalmap
 * ON/OFF:		N or M

Main light follow camera:
 * ON/OFF: 		Q or E

Rendermode with GBuffers:
 * Only textures: 	Numpad 0 or T 
 * With lights: 	Numpad 1 or Y
 * Normals:		Numpad 2 or G
 * Positions:		Numpad 3 or H

Back face culling:
 * ON/OFF:		(Numpad 7 or Numpad 4) || (U or J)

Gauss Filter:
 * ON/OFF:		(Numpad 8 or Numpad 5) || (I or K)

Bilateral Filter: 	
 * ON/OFF:		(Numpad 9 or Numpad 6) || (O or L)

Reset camera location
to (0,0,0):		R

Quit the game: 		Escape

