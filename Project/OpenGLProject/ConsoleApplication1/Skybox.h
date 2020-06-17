#pragma once

#include <string>
using namespace std;

#include "Program.h"
#include "Cube.h"
#include "Camera.h"
#include "ImageCubeTexture.h"

class Skybox
{
public: 
	Cube* mSkyBox = new Cube();
	Program* mSkyProg;
	ImageCubeTexture* mTexture;

	Skybox(string vs, string fs, string px, string nx, string pz, string nz, string py, string ny);

	void draw(Camera* cam);
};