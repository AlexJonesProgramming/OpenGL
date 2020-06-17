#pragma once

#include "Skybox.h"
using namespace std;

Skybox::Skybox(string vs, string fs, string px, string nx, string pz, string nz, string py, string ny)
{
	mSkyProg = new Program(vs, fs, { "color" });
	mTexture = new ImageCubeTexture({ px, nx, pz, nz, py, ny });

}
void Skybox::draw(Camera* cam)
{
	mSkyProg->use();
	cam->draw(mSkyProg);
	mSkyProg->setUniform("gWVP", translation(cam->eye.xyz()) * cam->viewmatrix*cam->projmatrix);
	mSkyProg->setUniform("gCubemapTexture", mTexture);
	mSkyBox->draw(mSkyProg);
}