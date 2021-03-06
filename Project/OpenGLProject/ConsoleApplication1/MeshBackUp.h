#pragma once 
#include "Program.h"
#include "ImageTexture.h"
#include "SolidTexture.h"

class Mesh{
    public:
    
    string filename;
    
    class Material{
        public:
        string name;
        Texture* texture = NULL;
        Texture* emitmap = NULL;
        Texture* specularmap = NULL;
        Texture* reflmap = NULL;
    };
    vector<Material> materials;
    vector<pair<int,int> > groups;
    int numvertices;
    int numindices;
    GLuint vao;
    
    Mesh(string filename){
        
        this->filename = filename;
        
        ifstream fp(filename,ios::binary);
        if(!fp)
            throw runtime_error("Cannot open file "+filename);
            
        string line;
        
        getline(fp,line);
        
        if(line != "mesh_01s" )
            throw runtime_error("Incorrect mesh format: "+line);
            
        GLuint tmp[1];
        glGenVertexArrays(1,tmp);
        this->vao = tmp[0];
        glBindVertexArray(this->vao);

        
        while(true){
            getline(fp,line);
            if( fp.fail() ){
                assert(0);
            }
            istringstream iss(line);
            string word;
            iss >> word;

            if( iss.fail() ){
            }
            else if( word == "num_vertices" )
                iss >> this->numvertices;
            else if( word == "num_indices" )
                iss >> this->numindices;
            else if( word == "positions"){
                vector<char> pdata;
                pdata.resize(this->numvertices*4*3);
                fp.read(&pdata[0],pdata.size());
                glGenBuffers(1,tmp);
                GLuint vbuff = tmp[0];
                glBindBuffer(GL_ARRAY_BUFFER,vbuff);
                glBufferData(GL_ARRAY_BUFFER,pdata.size(),&pdata[0],GL_STATIC_DRAW);
                glEnableVertexAttribArray(Program::POSITION_INDEX);
                glVertexAttribPointer(Program::POSITION_INDEX,3,GL_FLOAT,false,3*4,0);
            }
            else if( word == "texcoords"){
                vector<char> tdata;
                tdata.resize(this->numvertices*4*2);
                fp.read(&tdata[0],tdata.size());
                glGenBuffers(1,tmp);
                GLuint tbuff = tmp[0];
                glBindBuffer(GL_ARRAY_BUFFER,tbuff);
                glBufferData(GL_ARRAY_BUFFER,tdata.size(),&tdata[0],GL_STATIC_DRAW);
                glEnableVertexAttribArray(Program::TEXCOORD_INDEX);
                glVertexAttribPointer(Program::TEXCOORD_INDEX,2,GL_FLOAT,false,2*4,0);
            }
            else if( word == "normals"){
                vector<char> ndata;
                ndata.resize(this->numvertices*4*3);
                fp.read(&ndata[0],ndata.size());
                glGenBuffers(1,tmp);
                GLuint nbuff = tmp[0];
                glBindBuffer(GL_ARRAY_BUFFER,nbuff);
                glBufferData(GL_ARRAY_BUFFER,ndata.size(),&ndata[0],GL_STATIC_DRAW);
                glEnableVertexAttribArray(Program::NORMAL_INDEX);
                glVertexAttribPointer(Program::NORMAL_INDEX,3,GL_FLOAT,false,3*4,0);
            }
            else if( word == "indices"){
                vector<char> idata;
                idata.resize(this->numindices*4);
                fp.read(&idata[0],idata.size());
                glGenBuffers(1,tmp);
                GLuint ibuff = tmp[0];
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibuff);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,idata.size(),&idata[0],GL_STATIC_DRAW);

            }
            else if( word == "material" ){
                int start,num;
                string mtlname;
                float Kdr,Kdg,Kdb;
                iss >> start >> num >> mtlname >> Kdr >> Kdg >> Kdb;
                Material m;
                m.name = mtlname;
                while(1){
                    string key,value;
                    iss >> key >> value;
                    
                    if( iss.fail() )
                        break;
                        
                    if( key == "map_Kd" )
                        m.texture = new ImageTexture(value);
                    else if( key == "map_Ks" )
                        m.specularmap = new ImageTexture(value);
                    else if( key == "map_Ke" )
                        m.emitmap = new ImageTexture(value);
                    else if( key == "refl" )
                        m.reflmap = new ImageTexture(value);
                }
                
                if( !m.texture )
                    m.texture = new SolidTexture( Kdr,Kdg,Kdb,1.0);
                if( !m.specularmap )
                    m.specularmap = new SolidTexture( 1,1,1,1 );
                if( !m.emitmap )
                    m.emitmap = new SolidTexture(0,0,0,1);
                if( !m.reflmap )
                    m.reflmap = new SolidTexture(0,0,0,1);
                    
                materials.push_back(m);
                groups.push_back(make_pair(start,num));
            }
            else if( word == "end")
                break;
            else{
                cerr << word << "\n";
                assert(0);
            }
        }
 
        glBindVertexArray(0);
    }

    void draw(Program* prog){
		glBindVertexArray(this->vao);
        for(unsigned i=0;i<groups.size();++i){
            prog->setUniform("tex",materials[i].texture);
            prog->setUniform("etex",materials[i].emitmap);
            prog->setUniform("stex",materials[i].specularmap);
            prog->setUniform("rtex",materials[i].reflmap);
            glDrawElements(GL_TRIANGLES, groups[i].second, GL_UNSIGNED_INT,
                reinterpret_cast<GLvoid*>(groups[i].first*4) );
        }
        prog->checkUninitialized();
    }
};
        
