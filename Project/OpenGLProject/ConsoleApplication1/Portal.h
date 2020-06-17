#pragma once

#include "Square.h"
#include "Framebuffer.h"

class Portal{
    public:
    
    mat4 matrix;
    Square* sq;
    int which;
    Framebuffer2D* fbo;
    vec4 border;
    float db;
    
    Portal(int which){
        this->which=which;
        sq = new Square();
        fbo = new Framebuffer2D(512,512);
        if( which == 0 )
            border = vec4(1.0,0.2,0.0,0.1);
        else
            border = vec4(0.1,0.8,0.4,0.1);
        db = randrange(0.0001,0.0002);
    }
    
    void update(int elapsed){
        border.w += elapsed * db;
        if( border.w < 0 ){
            border.w=0;
            db=abs(db);
        }
        else if( border.w > 0.1){
            border.w = 0.1;
            db=-abs(db);
        }
    }
    
    void draw(Program* prog){
        prog->setUniform("border",border);
        prog->setUniform("worldMatrix",matrix);
        prog->setUniform("tex",fbo->texture);
        sq->draw(prog);
    }
    
    void set_basis(const vec4& pos, const vec4& U, const vec4& V, const vec4& W){
        matrix = mat4(
                U.x, U.y, U.z, 0,
                V.x, V.y, V.z, 0,
                W.x, W.y, W.z, 0,
                0,0,0,1
            ) * translation(pos);
    }
    
    //transform the given coordinate system into this portal's basis.
    void rebase(
        const vec4& origin, const vec4& U, const vec4& V, const vec4& W,
        vec4& o, vec4& UU, vec4& VV, vec4& WW){
            
        mat4& M(matrix);
        mat4 N( 
            M[0][0], M[0][1], M[0][2],0,
            M[1][0], M[1][1], M[1][2],0,
            M[2][0], M[2][1], M[2][2],0,
            0,0,0,1
        );
        //FIXME: rewrite to eliminate the transpose
        N = transpose(N);
        N = translation( -vec3(M[3][0],M[3][1],M[3][2]) ) * N ;
        o = origin * N;
        UU = U*N;
        VV = V*N;
        WW = W*N;
    }
};
