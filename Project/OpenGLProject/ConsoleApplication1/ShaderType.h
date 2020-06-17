#pragma once

using namespace std; 

#include "glfuncs.h"
#include <SDL.h>
#include "Program.h"
#include "Framebuffer.h"
#include "Square.h"
#include "Camera.h"
#include "World.h"


void compute_blur_coefficients(vector<float>& coefficients, int r, bool box)
{

	coefficients.resize(2 * r + 1);

	if (box) {
		for (unsigned i = 0; i<coefficients.size(); ++i)
			coefficients[i] = 1.0 / coefficients.size();
	}
	else {
		float sigma = r / 3.0;
		for (unsigned i = 0; i<coefficients.size(); ++i) {
			float tmp = -r + i*1.0 / (coefficients.size() - 1) * 2 * r;
			tmp = tmp*tmp;
			tmp = tmp / (-2.0*sigma*sigma);
			tmp = exp(tmp);
			tmp = tmp / (sigma * sqrt(2 * 3.14159265358979323));
			coefficients[i] = tmp;
		}
		float sum = 0;
		for (unsigned i = 0; i<coefficients.size(); ++i)
			sum += coefficients[i];
		for (unsigned i = 0; i<coefficients.size(); ++i)
			coefficients[i] /= sum;
	}

	coefficients.resize(49);

}

class ShaderType 
{

public:
	Program* postprocprog;
	Framebuffer2D* fbo1;
	Framebuffer2D* fbo2;
	Square* usq;
	vector<float> coefficients;
	int blur_radius;
	bool boxblur;

	ShaderType(string vs, string fs, int radius, bool box) 
	{
		postprocprog = new Program(vs, fs, {"color"});
		usq = new Square();
		fbo1 = new Framebuffer2D(1024, 1024, 2);
		fbo2 = new Framebuffer2D(1024, 1024, 2);
		blur_radius = radius;
		boxblur = box;
		compute_blur_coefficients(coefficients, blur_radius, boxblur);
	}

	void setUp() { fbo1->bind(); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void draw()
	{
		compute_blur_coefficients(coefficients, blur_radius, boxblur);
		fbo1->unbind();

		
		//"ping pong" blur: Horizontal blur to fbo 2
		postprocprog->use();
		postprocprog->setUniform("blur_kernel[0]", coefficients);
		postprocprog->setUniform("kernel_size", int(2 * blur_radius + 1)); //(int)coefficients.size());
		postprocprog->setUniform("blur_radius", (float)blur_radius); //(int)coefficients.size());
		
		postprocprog->setUniform("deltas", vec2(1, 0));
		fbo2->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		postprocprog->setUniform("tex", fbo1->texture);
		usq->draw(postprocprog);
		fbo2->unbind();
		fbo1->texture->unbind();
		
		//vertical blur to fbo 1 again
		//for this example, we could draw directly to the screen
		//instead of rendering to fbo1, but in general,
		//we wouldn't be rendering the blur directly to the screen --
		//we'd be processing it more first
		fbo1->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		postprocprog->setUniform("deltas", vec2(0, 1));
		postprocprog->setUniform("tex", fbo2->texture);
		usq->draw(postprocprog);
		fbo1->unbind();
		fbo2->texture->unbind();
		//render to the screen
		//this works because no FBO is bound so it can draw to the screen.
		
		postprocprog->setUniform("deltas", vec2(0, 0));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		postprocprog->setUniform("tex", fbo1->texture);
		usq->draw(postprocprog);
		fbo1->texture->unbind();
	}
};