/*
 *	TERRAINMAP
 *	By: Alexander Guerrero
 *	Date: 22/11/2015
 */

 #include <GLUT/glut.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <iostream>
 #include <time.h>
 #include <math.h> 

 #include "Vector3D.h"

 #define PI 3.14159265  

 using namespace std;

int windowSizeX = 800;
int windowSizeY = 800;

int mapSize; // set in main to mapSize + 1 for extra column 
			 // and row to make grid verts

float** vertsArr; // pointer for 2D-array of vertices
Vector3D** vertNorms; // pointer for 2D-array of vertex normals

float camPos[] = {0.0, 0.0, 0.0}; // position set in main(), depends on user input

bool lightOn = false;
float light0Pos[] = {0.0, 0.0, 0.0, 1.0};
float light1Pos[] = {0.0, 0.0, 0.0, 1.0};

bool gourShadingOn = true; // for toggling between flat and gouraud shading

float xRotation = 0.0f;
float yRotation = 0.0f;

float maxHeight = 0.0f;

char viewMode = 'w'; // s = solid, w = wireframe


void initVertArr(int size)
{	/*
	 * initializes vertsArr & vertNorms to user defined size
	 * and set value to 0.0
	 */
	vertsArr = new float*[size];
	vertNorms = new Vector3D*[size];

	int i; // i = x-dimension
	int j; // j = z-dimension

	for (i = 0; i < size; i++)
	{
		vertsArr[i] = new float[size]; 
		vertNorms[i] = new Vector3D[size];

		for (j = 0; j < size; j++)
		{
			vertsArr[i][j] = 0.0f;
			vertNorms[i][j] = Vector3D();
		}
	}
}


void drawPolygonMap()
{	// for all grid "cells"
	float height;
	Vector3D currentColour = Vector3D();

	for (int x = 0; x < mapSize; x++)
	{
		for (int z = 0; z < mapSize; z++)
		{	

			if (gourShadingOn)
			{
				glBegin(GL_QUADS);
					glNormal3f(vertNorms[x][z].x, vertNorms[x][z].y, vertNorms[x][z].z);
					// colour vert based on height
					height = (vertsArr[x][z] / (maxHeight*1.5)) + 0.15;
					currentColour = Vector3D(height, height, height);
					glColor3f(currentColour.x, currentColour.y, currentColour.z);
					glVertex3f(x, vertsArr[x][z], z);

					glNormal3f(vertNorms[x][z+1].x, vertNorms[x][z+1].y, vertNorms[x][z+1].z);
					height = (vertsArr[x][z+1] / (maxHeight*1.5)) + 0.15;
					currentColour = Vector3D(height, height, height);
					glColor3f(currentColour.x, currentColour.y, currentColour.z);
					glVertex3f(x, vertsArr[x][z+1], z+1);

					glNormal3f(vertNorms[x+1][z+1].x, vertNorms[x+1][z+1].y, vertNorms[x+1][z+1].z);
					height = (vertsArr[x+1][z+1] / (maxHeight*1.5)) + 0.15;
					currentColour = Vector3D(height, height, height);
					glColor3f(currentColour.x, currentColour.y, currentColour.z);
					glVertex3f(x+1, vertsArr[x+1][z+1], z+1);

					glNormal3f(vertNorms[x+1][z].x, vertNorms[x+1][z].y, vertNorms[x+1][z].z);
					height = (vertsArr[x+1][z] / (maxHeight*1.5)) + 0.15;
					currentColour = Vector3D(height, height, height);
					glColor3f(currentColour.x, currentColour.y, currentColour.z);
					glVertex3f(x+1,	vertsArr[x+1][z], z);
				glEnd();
				
			} else {

				// calc face normal for flat shading
				Vector3D A = Vector3D(0.0, (vertsArr[x][z+1] - vertsArr[x][z]), 1.0);
				Vector3D B = Vector3D(1.0, (vertsArr[x+1][z] - vertsArr[x][z]), 0.0);

				Vector3D crossProd = Vector3D( (A.y * B.z) - (A.z * B.y), 
									   		   (A.z * B.x) - (A.x * B.z), 
									   		   (A.x * B.y) - (A.y * B.x)  );

				glBegin(GL_QUADS);
					glNormal3f(crossProd.x, crossProd.y, crossProd.z);
					// colour vert based on height
					height = (vertsArr[x][z] / (maxHeight*1.5)) + 0.15;
					currentColour = Vector3D(height, height, height);
					glColor3f(currentColour.x, currentColour.y, currentColour.z);
					glVertex3f(x, vertsArr[x][z], z);

					height = (vertsArr[x][z+1] / (maxHeight*1.5)) + 0.15;
					currentColour = Vector3D(height, height, height);
					glColor3f(currentColour.x, currentColour.y, currentColour.z);
					glVertex3f(x, vertsArr[x][z+1], z+1);

					height = (vertsArr[x+1][z+1] / (maxHeight*1.5)) + 0.15;
					currentColour = Vector3D(height, height, height);
					glColor3f(currentColour.x, currentColour.y, currentColour.z);
					glVertex3f(x+1, vertsArr[x+1][z+1], z+1);

					height = (vertsArr[x+1][z] / (maxHeight*1.5)) + 0.15;
					currentColour = Vector3D(height, height, height);
					glColor3f(currentColour.x, currentColour.y, currentColour.z);
					glVertex3f(x+1,	vertsArr[x+1][z], z);
				glEnd();
			}
		}
	}
}


Vector3D normAvg( Vector3D fnorm1, Vector3D fnorm2,
				  Vector3D fnorm3, Vector3D fnorm4 )
{/*
  * Takes 4 face normals and calcs the coresponding
  * vertex normal
  */
  Vector3D vertNorm = Vector3D(fnorm1.x + fnorm2.x + fnorm3.x + fnorm4.x,
  							   fnorm1.y + fnorm2.y + fnorm3.y + fnorm4.y,
  							   fnorm1.z + fnorm2.z + fnorm3.z + fnorm4.z );
  return vertNorm;
}

void calcVertNorms()
{
	Vector3D NWfaceNorm = Vector3D();
	Vector3D NEfaceNorm = Vector3D();
	Vector3D SEfaceNorm = Vector3D();
	Vector3D SWfaceNorm = Vector3D();

	Vector3D vertNorm = Vector3D();

	// A & B for face norm calc
	Vector3D A = Vector3D();
	Vector3D B = Vector3D();

	for (int x = 0; x <= mapSize; x++)
	{
		for (int z = 0; z <= mapSize; z++)
		{	/*
			 *	CORNER CASES
			 */ 
			if(x == 0 && z == 0) // top left
			{	
				NWfaceNorm = Vector3D(0.0, 1.0, 0.0);
				NEfaceNorm = Vector3D(0.0, 1.0, 0.0);
				SWfaceNorm = Vector3D(0.0, 1.0, 0.0);

				A = Vector3D(0, (vertsArr[x][z+1] - vertsArr[x][z]), 1);
				B = Vector3D(1, (vertsArr[x+1][z] - vertsArr[x][z]), 0);

				SEfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) );

				vertNorms[x][z] = normAvg( NWfaceNorm, NEfaceNorm,
										   SEfaceNorm, SWfaceNorm );
			} 

			else if(x == 0 && z == mapSize) // bottom left
			{
				NWfaceNorm = Vector3D(0.0, 1.0, 0.0);
				SEfaceNorm = Vector3D(0.0, 1.0, 0.0);
				SWfaceNorm = Vector3D(0.0, 1.0, 0.0);

				A = Vector3D(0, (vertsArr[x][z] - vertsArr[x][z-1]), 1);
				B = Vector3D(1, (vertsArr[x+1][z-1] - vertsArr[x][z-1]), 0);

				NEfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) );

				vertNorms[x][z] = normAvg( NWfaceNorm, NEfaceNorm,
										   SEfaceNorm, SWfaceNorm ); 
			}

			else if(x == mapSize && z == 0) // top right
			{
				NWfaceNorm = Vector3D(0.0, 1.0, 0.0);
				NEfaceNorm = Vector3D(0.0, 1.0, 0.0);
				SEfaceNorm = Vector3D(0.0, 1.0, 0.0);

				A = Vector3D(0, (vertsArr[x-1][z+1] - vertsArr[x-1][z]), 1);
				B = Vector3D(1, (vertsArr[x][z] - vertsArr[x-1][z]), 0);

				SWfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) );

				vertNorms[x][z] =  normAvg( NWfaceNorm, NEfaceNorm,
										   SEfaceNorm, SWfaceNorm );
			}
			
			else if(x == mapSize && z == mapSize) // bottom right
			{
				NEfaceNorm = Vector3D(0.0, 1.0, 0.0);
				SEfaceNorm = Vector3D(0.0, 1.0, 0.0);
				SWfaceNorm = Vector3D(0.0, 1.0, 0.0);

				A = Vector3D(0, (vertsArr[x-1][z] - vertsArr[x-1][z-1]), 1);
				B = Vector3D(1, (vertsArr[x][z-1] - vertsArr[x-1][z-1]), 0);

				NWfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) );

				vertNorms[x][z] =  normAvg( NWfaceNorm, NEfaceNorm,
										   SEfaceNorm, SWfaceNorm );
			}

			/*
			 *	BOARDER CASES
			 */ 
			else if(x == 0 && (z < mapSize && z > 0)) // left
			{
				NWfaceNorm = Vector3D(0.0, 1.0, 0.0);
				SWfaceNorm = Vector3D(0.0, 1.0, 0.0);

				A = Vector3D(0, (vertsArr[x][z] - vertsArr[x][z-1]), 1);
				B = Vector3D(1, (vertsArr[x+1][z-1] - vertsArr[x][z-1]), 0);
				NEfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) ); 

				A = Vector3D(0, (vertsArr[x][z+1] - vertsArr[x][z]), 1);
				B = Vector3D(1, (vertsArr[x+1][z] - vertsArr[x][z]), 0);	
				SEfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) ); 

				vertNorms[x][z] =  normAvg( NWfaceNorm, NEfaceNorm,
										   SEfaceNorm, SWfaceNorm );	
			} 

			else if(x == mapSize && (z < mapSize && z > 0)) // right
			{
				NEfaceNorm = Vector3D(0.0, 1.0, 0.0);
				SEfaceNorm = Vector3D(0.0, 1.0, 0.0);

				A = Vector3D(0, (vertsArr[x-1][z] - vertsArr[x-1][z-1]), 1);
				B = Vector3D(1, (vertsArr[x][z-1] - vertsArr[x-1][z-1]), 0);
				NWfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) ); 

				A = Vector3D(0, (vertsArr[x-1][z+1] - vertsArr[x-1][z]), 1);
				B = Vector3D(1, (vertsArr[x][z] - vertsArr[x-1][z]), 0);	
				SWfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) ); 

				vertNorms[x][z] =  normAvg( NWfaceNorm, NEfaceNorm,
										   SEfaceNorm, SWfaceNorm );
			} 

			else if(z == 0 && (x < mapSize && x > 0)) // top
			{
				NWfaceNorm = Vector3D(0.0, 1.0, 0.0);
				NEfaceNorm = Vector3D(0.0, 1.0, 0.0);

				A = Vector3D(0, (vertsArr[x-1][z+1] - vertsArr[x-1][z]), 1);
				B = Vector3D(1, (vertsArr[x][z] - vertsArr[x-1][z]), 0);
				SWfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) ); 

				A = Vector3D(0, (vertsArr[x][z+1] - vertsArr[x][z]), 1);
				B = Vector3D(1, (vertsArr[x+1][z] - vertsArr[x][z]), 0);	
				SEfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) ); 

				vertNorms[x][z] =  normAvg( NWfaceNorm, NEfaceNorm,
										   SEfaceNorm, SWfaceNorm );
			} 

			else if(z == mapSize && (x < mapSize && x > 0)) // bottom
			{
				SEfaceNorm = Vector3D(0.0, 1.0, 0.0);
				SWfaceNorm = Vector3D(0.0, 1.0, 0.0);

				A = Vector3D(0, (vertsArr[x-1][z] - vertsArr[x-1][z-1]), 1);
				B = Vector3D(1, (vertsArr[x][z-1] - vertsArr[x-1][z-1]), 0);
				NWfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) ); 

				A = Vector3D(0, (vertsArr[x][z] - vertsArr[x][z-1]), 1);
				B = Vector3D(1, (vertsArr[x+1][z-1] - vertsArr[x][z-1]), 0);	
				NEfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) ); 

				vertNorms[x][z] =  normAvg( NWfaceNorm, NEfaceNorm,
										   SEfaceNorm, SWfaceNorm );

			} else { 
			/*  
			 * GENERAL CASE
			 */
				A = Vector3D(0, (vertsArr[x][z-1] - vertsArr[x][z]), -1);
				B = Vector3D(-1, (vertsArr[x-1][z] - vertsArr[x][z]), 0);
				NWfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) ); 

				A = Vector3D(1, (vertsArr[x+1][z] - vertsArr[x][z]), 0);
				B = Vector3D(0, (vertsArr[x][z-1] - vertsArr[x][z]), -1);	
				NEfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) );

				A = Vector3D(0, (vertsArr[x][z+1] - vertsArr[x][z]), 1);
				B = Vector3D(1, (vertsArr[x+1][z] - vertsArr[x][z]), 0);	
				SEfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) );

				A = Vector3D(-1, (vertsArr[x-1][z] - vertsArr[x][z]), 0);
				B = Vector3D(0, (vertsArr[x][z+1] - vertsArr[x][z]), 1);
				SWfaceNorm = Vector3D( (A.y * B.z) - (A.z * B.y), 
								   	   (A.z * B.x) - (A.x * B.z), 
								   	   (A.x * B.y) - (A.y * B.x) ); 

				vertNorms[x][z] =  normAvg( NWfaceNorm, NEfaceNorm,
										   SEfaceNorm, SWfaceNorm ); 
			}
		}
	}
}


float setHeight(float centreX, float centreZ, float pointX, 
				float pointZ, float circRadius, float heightChange)
{	/*
	 * http://www.lighthouse3d.com/opengl/terrain/index.php?circles
	 */ 
	float distToCentre = sqrt( pow(centreX - pointX, 2.0) 
							  + pow(centreZ - pointZ, 2.0) );

	float pd = distToCentre * 2.0 / (circRadius * 2.0);
	float height = 0.0;

	if (fabs(pd) <= 1.0)
	{
		height = heightChange + cos(pd * PI) * heightChange;
	}
	// check to make sure we are not setting a lower height value
	if(vertsArr[(int)pointX][(int)pointZ] != 0 
		&& vertsArr[(int)pointX][(int)pointZ] > height)
	{
		height = vertsArr[(int)pointX][(int)pointZ];
	}

	return height;
}


void circleAlg()
{	/*
	 * http://www.lighthouse3d.com/opengl/terrain/index.php?circles
	 */ 
	int randX, randZ, heightChange, i, j;
	int circRadius, radiusVar, heightVar1, heightVar2;
	int iterations;

	// change number of circle iterations based on mapSize
	if(mapSize >= 200){iterations = mapSize;}
	else if(mapSize <= 100 < 200){iterations = (3 * mapSize) / 4;}
	else{iterations = mapSize / 2;}

	for (i = 0; i < iterations; i++)
	{	
		// random centre point coords, height, and circle radius
		radiusVar = mapSize * 0.08;
		heightVar1 = mapSize * 0.02;
		heightVar2 = mapSize * 0.1;

		circRadius = rand() % radiusVar + radiusVar;
		randX = rand() % (mapSize - circRadius*2) + circRadius; 
		randZ = rand() % (mapSize - circRadius*2) + circRadius;
		heightChange = rand() % heightVar2 + heightVar1;
		/*
		 * Find border of half of circle using a modified:
		 * https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
		 */
  		int x = circRadius;
 		int z = 0;
 		// Decision criterion divided by 2 evaluated at x=r, y=0
 		int decisionOver2 = 1 - x;   

 		while( z <= x )
 		{ 	/* 
 			 * when border vert is found, loop through entire x-column of vertsArr
 			 * (aka full length, cross section of cirlce), and set height
			 */
   			for (j = -x; j < x+1; j++)
   			{	
   				vertsArr[j + randX][z + randZ]
   				= setHeight(randX, randZ, j + randX, z + randZ,
   									circRadius, heightChange); // Octant 1
   			}

   			if (x == circRadius)
   			{
   				vertsArr[z + randX][x + randZ] 
   				= setHeight(randX, randZ, z + randX, x + randZ, 
   									circRadius, heightChange); // Octant 2
   			} else {
   				for (j = -z; j < z+1; j++)
   				{
   					vertsArr[j + randX][x + randZ] 
   					= setHeight(randX, randZ, j + randX, x + randZ,
   									circRadius, heightChange); // Octant 2
   				}
   			}

 		  	for (j = -x; j < x+1; j++)
   			{	
 		  		vertsArr[j + randX][-z + randZ] 
 		  		= setHeight(randX, randZ, j + randX, -z + randZ, 
 		  							circRadius, heightChange); // Octant 8
 			}

 			if (x == circRadius)
 			{
 				vertsArr[z + randX][-x + randZ] 
 				= setHeight(randX, randZ, z + randX, -x + randZ, 
 									circRadius, heightChange); // Octant 7
	 		} else {
	 			for (j = -z; j < z+1; j++)
	   			{
	 		  		vertsArr[j + randX][-x + randZ] 
	 		  		= setHeight(randX, randZ, j + randX, -x + randZ, 
	 		  						circRadius, heightChange); // Octant 7
	 		  	}
	 		}
 			z++;

  			if (decisionOver2<=0)
  			{
   	  	 		decisionOver2 += 2 * z + 1; // Change in decision criterion for z -> z+1
   			} else {
     			x--;
      			decisionOver2 += 2 * (z - x) + 1; // Change for z -> z+1, x -> x-1
    		}
  		} 		
	}
}


void drawAxis()
{
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(0,0,0);
		glVertex3f(10,0,0);
		glColor3f(0,1,0);
		glVertex3f(0,0,0);
		glVertex3f(0,10,0);
		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,10);
	glEnd();
}


void keyboard(unsigned char key, int x, int y)
{	
	// for lighting
	float amb[4]  = {1, 1, 1, 1};
	float diff[4] = {1, 1, 1, 1};
	float spec[4] = {1, 1, 1, 1};

	float lightMvmnt = mapSize * 0.1;

	switch (key)
	{
		case 'q':
		case 27:
			exit (0);
			break;

		case 'w':
		case 'W':
			if (viewMode == 's')
			{	
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
				viewMode = 'w';
			} else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
				viewMode = 's';
			}
			break;

		case 'l':
		case 'L':

			if (lightOn)
			{
				lightOn = false;
				glDisable(GL_LIGHTING);
			} else {
				lightOn = true;
				glEnable(GL_LIGHTING);
				// Enable 2 light sources
				glEnable(GL_LIGHT0);
				glEnable(GL_LIGHT1);

				glEnable(GL_NORMALIZE);

				glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
				glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
				glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
				glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

				glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
				glLightfv(GL_LIGHT1, GL_DIFFUSE, diff);
				glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
				glLightfv(GL_LIGHT1, GL_SPECULAR, spec);
			}

			break;

		case 'y': // move light AWAY from screen
		case 'Y': 
			if(light0Pos[2] > -mapSize * 3)
			{
				light0Pos[2] -= lightMvmnt ;
				light1Pos[2] -= lightMvmnt;
			}
			break;

		case 'g': // move light TOWARDS from screen
		case 'G':
			if(light0Pos[0] > -mapSize * 3)
			{
				light0Pos[0] -= lightMvmnt;
				light1Pos[0] -= lightMvmnt;
			}
			break;

		case 'h': // move light RIGHT
		case 'H':
			if(light0Pos[2] < mapSize * 3)
			{
				light0Pos[2] += lightMvmnt;
				light1Pos[2] += lightMvmnt;
			}
			break;

		case 'j': // move light RIGHT
		case 'J':
			if(light0Pos[0] < mapSize * 3)
			{
				light0Pos[0] += lightMvmnt;
				light1Pos[0] += lightMvmnt;
			}
			break;

		case '-': // zoom out
		case '_':
			camPos[1] += mapSize * 0.05;
			camPos[2] += (mapSize * 1.5) * 0.05;
			break;

		case '=': // zoom in
		case '+':
			camPos[1] -= mapSize * 0.05;
			camPos[2] -= (mapSize * 1.5) * 0.05;
			break;

		case 'e':
		case 'E':
			if (gourShadingOn)
			{
				glShadeModel(GL_FLAT);
				gourShadingOn = false;
			} else {
				calcVertNorms();
				glShadeModel(GL_SMOOTH);
				gourShadingOn = true;
			}
			break;

		case 'r':
		case 'R':
				// generate new map
				// re-initialize verts and apply circle alg again
				initVertArr(mapSize + 1);
				circleAlg();
				calcVertNorms();

				// reset light poisitioning
				light0Pos[0] = -mapSize;
				light0Pos[1] = mapSize * 0.2;
				light0Pos[2] = mapSize;

				light1Pos[0] = mapSize;
				light1Pos[1] = mapSize * 0.2;
				light1Pos[2] = -mapSize;

				glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
				glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
			break;
	}
	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	/* arrow key presses move the map */
	switch(key)
	{
		case GLUT_KEY_LEFT:
			yRotation -= 11.25;
			break;

		case GLUT_KEY_RIGHT:
			yRotation += 11.25;
			break;

		case GLUT_KEY_UP:
			// so user can't flip grid upside down
			if (xRotation < 45){xRotation += 11.25;}
			break;

		case GLUT_KEY_DOWN:
			// so user can't flip grid upside down
			if (xRotation > -135){xRotation -= 11.25;}
			break;
	}
	glutPostRedisplay();
}

void init(void)
{
	glClearColor(0, 0, 0, 0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, (mapSize-1) * 3); 

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


void display(void)
{	// set material properties
	float m_amb[] = {0.0107, 0.087, 0.0107, 0.55};
	float m_dif[] = {0.07568, 0.61424, 0.07568, 0.55};
	float m_spec[] = {0.633, 0.727811, 0.633, 0.55};
	float shiny = 76.8;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,1,0);

	// update light positions
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_dif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);

	//drawAxis(); // for testing purposes
	glRotatef(xRotation, 1, 0.0, 0.0);
	glRotatef(yRotation, 0.0, 1, 0.0);

	// tranlate drawing of matrix so it displays in centre
	glTranslatef(-(mapSize-1)/2, 0, -(mapSize-1)/2);
	drawPolygonMap();
	
	glutSwapBuffers();
}

/* main function - program entry point */
int main(int argc, char** argv)
{	
	// Get user input for map size (NxN grid)

	cout << "\n\n\n\n\n\n\n**********************************************\n";
	cout << "WELCOME TO MY TERRAIN MAP!\n"; 
	cout << "By: Alex Guerrero\n";
	cout << "**********************************************\n";
	cout << "CONTROLS:\n\n";
	cout << "* w - toggle wireframe or solid view mode\n";
	cout << "* e - toggle flat or gouraud (smooth) shading\n";
	cout << "* l - toggle lights\n";
	cout << "\t-> y, g, h, j - move lights position\n";
	cout << "* arrow keys - rotate map\n";
	cout << "* -/_ - zoom out\n";
	cout << "* +/= - zoom in\n";
	cout << "* r - reset map\n";
	cout << "* q - quit\n";
	cout << "**********************************************\n\n";
	
	int userInput;

	// check user input
	bool check = true;
	while (check == true)
	{
		cout << "Please enter an integer from 50 - 300: ";
		cin >> userInput;
		if (userInput >= 50 && userInput <= 300)
		{
			mapSize = userInput;
			check = false;
			//adjust camera positions relative to map size
			camPos[1] = mapSize;
			camPos[2] = mapSize * 1.5;

			light0Pos[0] = -mapSize;
			light0Pos[1] = mapSize * 0.2;
			light0Pos[2] = mapSize;

			light1Pos[0] = mapSize;
			light1Pos[1] = mapSize * 0.2;
			light1Pos[2] = -mapSize;

			maxHeight = mapSize * 0.2; 

		} else {
			cout << "Invalid integer.\n";			

		}
	}

	initVertArr(mapSize + 1); // +1 because for the grid to be
							  // NxN need extra col and row of verts

	circleAlg();


	glutInit(&argc, argv); //starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowSize(windowSizeX, windowSizeY);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("TerrainMap");

	glutDisplayFunc(display);	
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	glEnable(GL_DEPTH_TEST);
	init();

	srand (time(NULL)); // makes rand() produce "more random" numbers
						// reference: http://www.cplusplus.com/reference/cstdlib/srand/

	glutMainLoop(); //starts the event loop

	return(0); //return may not be necessary on all compilers
}