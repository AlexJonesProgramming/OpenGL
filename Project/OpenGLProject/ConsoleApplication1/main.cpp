
#include "glfuncs.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <stdexcept>
#include <cassert>
#include <stdlib.h>
#include <ciso646>
#include <typeinfo>
#include <cmath>
#include <iostream>
#include <list>

using namespace std;

#include <SDL.h>
#include "Program.h"
#include "Mesh.h"
#include "math3d.h"
#include "Texture.h"
#include "Camera.h"
#include "Helicopter.h"
#include "Boxes.h"
#include "ShaderType.h"
#include "Skybox.h"
#include "Rings.h"


using namespace std;

void APIENTRY debugcallback( GLenum source, GLenum typ,
    GLuint id_, GLenum severity, GLsizei length,
    const GLchar* message, const void* obj ){
        
    static set<GLuint> warned;
    
    if( severity == GL_DEBUG_SEVERITY_LOW && warned.find(id_) != warned.end() )
        return;
    warned.insert(id_);

    cout << message << endl;
    if( source != GL_DEBUG_SOURCE_SHADER_COMPILER && severity == GL_DEBUG_SEVERITY_HIGH ){
        throw runtime_error("Fatal GL error.");
    }
    
}

//== start of the program ==========================
int main(int argc, char* argv[])
{
	//== Screen set up ===============================
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow(
		"HELL(ocopter) RINGS",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1808, 1017,
		SDL_WINDOW_OPENGL);
	if (!win) {
		throw runtime_error("Could not create window");
	}
	//== OpenGL set up ===============================
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GLContext rc = SDL_GL_CreateContext(win);
	if (!rc) {
		throw runtime_error("Cannot create GL context");
	}

	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, 1);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debugcallback, NULL);

	// == Vars ============================================================

	// shader stuff
	ShaderType* blur = new ShaderType("ppvs.txt", "ppfs.txt", 11, false);
	Skybox* mySkyBox = new Skybox("skyBoxvs.txt", "skyBoxfs.txt", "px.png", "nx.png", "pz.png", "nz.png", "py.png", "ny.png");
	Program* prog = new Program("vs.txt", "fs.txt", { "color" });
	Program* shadowProg = new Program("shadow.vert", "shadow.frag", { "color" });
	Camera* cam = new Camera();
	vec3 lightPos(-2, 50, 10);
	vec3 lightDir(0, -1, 0);
	Framebuffer2D* ShadowBuffer = new Framebuffer2D(1024, 1024, 1, GL_R32F, GL_FLOAT);
	// objects
	Rings* rings = new Rings();
	Mesh* meshGround = new Mesh("Ground.blend.ms3d.mesh");
	Helicopter* H = new Helicopter();

	// setting some gl stuff
	glClearColor(1, 0.0, 1, 1.0); // this sets the background color
	glEnable(GL_DEPTH_TEST); // we use this
	glDepthFunc(GL_LEQUAL);  // and this for the skybox to work
	// random things
	set<int> keys;
	int last = SDL_GetTicks();
	SDL_Event ev;
	float timer;
	int ringTracker = 0;

	//==============================================================================================================
	//=====                                        MAIN GAME LOOP                                              =====
	//==============================================================================================================

	while (1) {

		// quit on key down ==============================================
		while (1) {
			if (!SDL_PollEvent(&ev)) {
				break;
			}

			if (ev.type == SDL_QUIT) {
				exit(0);
			}
			else if (ev.type == SDL_KEYDOWN)
			{
				int k = ev.key.keysym.sym;
				keys.insert(k);
				if (k == SDLK_ESCAPE)
				{
					exit(0);
				}
			}
			else if (ev.type == SDL_KEYUP)
			{
				int k = ev.key.keysym.sym;
				keys.erase(k);
			}
			else if (ev.type == SDL_MOUSEBUTTONDOWN)
			{
				//print("mouse down:",ev.button.button,ev.button.x,ev.button.y);
			}
			else if (ev.type == SDL_MOUSEBUTTONUP)
			{
				//cout << ("mouse up:",ev.button.button,ev.button.x,ev.button.y);
			}
			else if (ev.type == SDL_MOUSEMOTION)
			{
				//int dx = ev.motion.xrel;
				//cam->turn(-0.01*dx);
			}
		}

		// time control ===================================================
		int now = SDL_GetTicks();
		float elapsed = now - last; // this is delta time
		last = now;

		// Heli movment input =============================================
		if (keys.find(SDLK_w) != keys.end())
			H->changeSpeed(1);
		if (keys.find(SDLK_s) != keys.end())
			H->changeSpeed(-1);
		if (keys.find(SDLK_a) != keys.end())
			H->turn(1, elapsed);
		if (keys.find(SDLK_d) != keys.end())
			H->turn(-1, elapsed);
		if (keys.find(SDLK_UP) != keys.end())
			H->changeGain(1);
		if (keys.find(SDLK_DOWN) != keys.end())
			H->changeGain(-1);
		if (keys.find(SDLK_i) != keys.end())
			H->changeCamAngle(1);
		if (keys.find(SDLK_k) != keys.end())
			H->changeCamAngle(-1);
		if (keys.find(SDLK_e) != keys.end())
			rings->currentRing = 0;
		bool showShadow = keys.find(SDLK_r) != keys.end();
		//== Updates ======================================================
		H->update(elapsed);
		rings->update(H, elapsed);
		blur->blur_radius = (rings->currentRing + 1) * 2;
		timer += elapsed / 1000.0f;
		cout << timer << "\n";
		if (timer > 30)
		{
			rings->currentRing = 0;
			timer = 0;
		}
		if (ringTracker != rings->currentRing)
		{
			timer = 0;
			ringTracker = rings->currentRing;
		}


		// == Drawing Code ================================================
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		float h = cam->hither;
		float y = cam->yon;
		
		if (!showShadow) {
			ShadowBuffer->bind();
		}

			shadowProg->use();
			if (rings->currentRing < 15) {
				lightPos = H->pos;
			}
			//cam->look_at(lightPos, H->pos, vec3(0, 1, 0)); // makes the cam be the light =)
			//cam->look_at(H->getCamPos(), H->pos, vec3(0, 1, 0));
			lightDir = (vec4(1,0,0,0) * axisRotation(vec4(0, 1, 0, 0), H->Yrot*3.141596/180)).xyz();
			cam->look_at(H->pos - lightDir, H->pos - lightDir *4, vec3(0, 1, 0));
			cam->draw(shadowProg);
			shadowProg->setUniform("hitheryon", vec3(h, y, y - h));
			shadowProg->setUniform("worldMatrix", mat4::identity() * scaling(50, 50, 50)  * translation(vec3(0, 0, 0)));
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			meshGround->draw(shadowProg);
			rings->draw(shadowProg);
			//H->draw(shadowProg);
			if (!showShadow) {
				ShadowBuffer->unbind();
			
		
			blur->setUp();
			

			prog->use();
			
			//cam->look_at(H->getCamPos(), H->pos, vec3(0, 1, 0));
			//cam->look_at(lightPos, H->pos, vec3(0, 1, 0)); // makes the cam be the light =)
			cam->look_at(H->pos - lightDir, H->pos - lightDir * 4, vec3(0, 1, 0));
			cam->draw(prog, "lightProjMatrix", "lightViewMatrix", "lightPos");
			prog->setUniform("lightHitheryon", vec3(h, y, y - h));
			prog->setUniform("lightColor", vec4(1, 1, 1, 0));
			prog->setUniform("shadowBuffer", ShadowBuffer->texture);
			prog->setUniform("doShadow", 1);
			cam->look_at(H->getCamPos(), H->pos, vec3(0, 1, 0)); // makes the cam look at the heli
			cam->draw(prog);
			prog->setUniform("worldMatrix", mat4::identity() * scaling(50, 50, 50)  * translation(vec3(0, 0, 0)));
			
			meshGround->draw(prog);
			rings->draw(prog);
			H->draw(prog);
			mySkyBox->draw(cam);

			blur->draw();
			
			ShadowBuffer->texture->unbind();
		}
		

		//== Error stuff ===============================================
		GLenum err = glGetError();
		while (err != GL_NO_ERROR) {
			cout << "GL error: " << hex << err << "\n";
			err = glGetError();
		}
		SDL_GL_SwapWindow(win);

	} //endwhile
	cout << "debugging pause";
	cin.get();
	return 0;// all good programs return 0 ;)
}


// Defining Statics ================================
Program* Program::active=NULL;
Texture* Texture::active_textures[128];   
int Square::vao = 0;
int RenderTarget::saved_viewport[4];
RenderTarget* RenderTarget::active_target = NULL;

