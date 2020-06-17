
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
#include <list>]

#include <SDL.h>
#include "Program.h"
#include "Mesh.h"
#include "math3d.h"
#include "Texture.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "ShaderType.h"
#include "Skybox.h"
#include <SDL_mixer.h>
#include "AudoManager.h"
#include "Arrow.h"

using namespace std;

//== no clue what this does ===========================
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
        "Album Title Goes Here",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		1808, 1017,
        SDL_WINDOW_OPENGL);
    if(!win){
        throw runtime_error("Could not create window");
    }

	//== OpenGL set up ===============================
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GLContext rc = SDL_GL_CreateContext(win);
    if(!rc){
        throw runtime_error( "Cannot create GL context" );
    }
            
    glDebugMessageControl(GL_DONT_CARE,GL_DONT_CARE,GL_DONT_CARE, 0, NULL, 1 );
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugcallback,NULL);
    
	// this sets the background color
	glClearColor(0.25, 0.0, 0.25, 1.0); // <-- purple :)
	glEnable(GL_DEPTH_TEST); // we use this
	glDepthFunc(GL_LEQUAL);  // and this for the skybox to work
	glEnable(GL_BLEND); // this and the one below allow for transparancy
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
	//== This is where we assign vars ======================================================
	Mesh* R = new Mesh("robot.ms3d.mesh");
	Program* prog = new Program("vs.txt", "fs.txt", { "color" });
	Camera* cam = new Camera();
    vec3 lightPos(-2,10,10);
    set<int> keys;
    int last=SDL_GetTicks();
    SDL_Event ev;
    int selectedbone=0;

	//== TESTING =============================================================================
	ShaderType* blur = new ShaderType("ppvs.txt", "ppfs.txt", 1, false);
	Skybox* mySkyBox = new Skybox("skyBoxvs.txt", "skyBoxfs.txt", "px.png", "nx.png", "pz.png", "nz.png", "py.png", "ny.png");
	Arrow* testArrow1 = new Arrow();
	testArrow1->importLevel("Songs/testSong.txt");
	bool start = false;
	int blurLevel = 1;

	// audio stuff ============================
	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
	{
		return false;
	}
	AudioManager* AM = new AudioManager("songOne.ogg", "songTwo.ogg");
	bool doingGood = true;

    
//==============================================================================================================
//=====                                        MAIN GAME LOOP                                              =====
//==============================================================================================================
	while(1){

		//code to move the robot and close the program on escape
        while(1){
            if(!SDL_PollEvent(&ev)){
                break;
            }
        
            if(ev.type == SDL_QUIT){
                exit(0);
            } else if( ev.type == SDL_KEYDOWN ){
                int k = ev.key.keysym.sym;
                keys.insert(k);
                if(k == SDLK_ESCAPE){
                    exit(0);
                }
                if( k >= SDLK_F1 && k <= SDLK_F4 ){
                    if( k == SDLK_F1){
                        R->boneRotations[selectedbone].x = 0;
                        R->boneRotations[selectedbone].y = 0;
                        R->boneRotations[selectedbone].z = 1;
                        R->boneRotations[selectedbone].w += 0.1;
                    }
                    if( k == SDLK_F2){
                        R->boneRotations[selectedbone].x = 0;
                        R->boneRotations[selectedbone].y = 0;
                        R->boneRotations[selectedbone].z = 1;
                        R->boneRotations[selectedbone].w -= 0.1;
                    }
                    if( k == SDLK_F3 && selectedbone < 31)
                        selectedbone++;
                    if( k == SDLK_F4 && selectedbone > 0)
                        selectedbone--;
                    cout << "Rotating bone " << selectedbone << ": " << R->boneRotations[selectedbone] << "\n";
                }
            } else if(ev.type == SDL_KEYUP){
                int k = ev.key.keysym.sym;
                keys.erase(k);
            } else if( ev.type == SDL_MOUSEBUTTONDOWN ){
                //print("mouse down:",ev.button.button,ev.button.x,ev.button.y);
            } else if(ev.type == SDL_MOUSEBUTTONUP){
                //cout << ("mouse up:",ev.button.button,ev.button.x,ev.button.y);
            } else if( ev.type == SDL_MOUSEMOTION ){
                //int dx = ev.motion.xrel;
                //cam->turn(-0.01*dx);
            }
        }
        
		// keeps track of the time for updates
        int now = SDL_GetTicks();
        float elapsed = now-last; //<- what I would call DeltaTime
        last=now;
        
		//== Input Stuff =============================
		if (keys.find(SDLK_RETURN) != keys.end())
		{
			AM->reset(); // we do this first because it takes the longest
			testArrow1->resetLevel();
			start = true;
			AM->play();
		}
		if (keys.find(SDLK_i) != keys.end())
		{
			testArrow1->importLevel("testSong3.txt");
		}
		if (keys.find(SDLK_k) != keys.end())
		{
			testArrow1->importLevel("testSong2.txt");
		}
		if (keys.find(SDLK_w) != keys.end())
		{
			if (!testArrow1->checkArrow("up") && doingGood)
			{
				AM->muffle();
				doingGood = false;
			}
			else if (testArrow1->checkArrow("up") && !doingGood)
			{
				AM->muffle();
				doingGood = true;
			}
		}
		if (keys.find(SDLK_a) != keys.end())
		{
			if (!testArrow1->checkArrow("left") && doingGood)
			{
				AM->muffle();
				doingGood = false;
			}
			else if (testArrow1->checkArrow("left") && !doingGood)
			{
				AM->muffle();
				doingGood = true;
			}
		}
		if (keys.find(SDLK_s) != keys.end())
		{
			if (!testArrow1->checkArrow("down") && doingGood)
			{
				AM->muffle();
				doingGood = false;
			}
			else if (testArrow1->checkArrow("down") && !doingGood)
			{
				AM->muffle();
				doingGood = true;
			}
		}
		if (keys.find(SDLK_d) != keys.end())
		{
			if (!testArrow1->checkArrow("right") && doingGood)
			{
				AM->muffle();
				doingGood = false;;
			}
			else if (testArrow1->checkArrow("right") && !doingGood)
			{
				AM->muffle();
				doingGood = true;
			}
		}
		if (keys.find(SDLK_UP) != keys.end())
		{
			if (blurLevel < 11)
			{
				blurLevel++;
				blur->blur_radius = blurLevel;
			}
		}
		if (keys.find(SDLK_DOWN) != keys.end())
		{
			if (blurLevel > 1)
			{
				blurLevel--;
				blur->blur_radius = blurLevel;
			}
		}
		/*
		if(keys.find(SDLK_w) != keys.end() )
            cam->walk(0.01*elapsed);
        if(keys.find(SDLK_s) != keys.end() )
            cam->walk(-0.01*elapsed);
        if(keys.find(SDLK_a) != keys.end() )
            cam->turn(0.001*elapsed);
        if(keys.find(SDLK_d) != keys.end() )
            cam->turn(-0.001*elapsed);
        if(keys.find(SDLK_i) != keys.end() )
            cam->strafe(0,0.001*elapsed,0);
        if(keys.find(SDLK_k) != keys.end() )
            cam->strafe(0,-0.001*elapsed,0);
        if(keys.find(SDLK_j) != keys.end() )
            cam->strafe(-0.001*elapsed,0,0);
        if(keys.find(SDLK_l) != keys.end() )
            cam->strafe(0.001*elapsed,0,0);
			*/
        
		//== Updates ============================================================
		if (start) { testArrow1->update(elapsed); }

		// == Drawing Code ================================================
		blur->setUp(); // binds a FBO to render to
        
        prog->use();
		prog->setUniform("worldMatrix", mat4::identity());
		prog->setUniform("lightPos", lightPos);
        cam->draw(prog);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
		testArrow1->draw(prog); // <===== TEST VAR
	
        //for debugging
        prog->setUniform("selectedbone",selectedbone);
		//this draws the robot with kenetics
		prog->setUniform("worldMatrix", mat4::identity()* translation(vec3(0.0,0.0,-5.0)));
        R->draw(prog);

		mySkyBox->draw(cam);	//this renders the skybox
		blur->draw();
        
		//== Error stuff ===============================================
        GLenum err = glGetError();
        while(err != GL_NO_ERROR ){
            cout << "GL error: " << hex << err << "\n";
            err = glGetError();
        }
        
        SDL_GL_SwapWindow(win);
            
    } //endwhile
    
    return 0; // all good programs return 0 ;)
}

// Defining Statics ================================
Program* Program::active=NULL;
Texture* Texture::active_textures[128];  
int Square::vao=NULL;
int RenderTarget::saved_viewport[4];
RenderTarget* RenderTarget::active_target=NULL;

