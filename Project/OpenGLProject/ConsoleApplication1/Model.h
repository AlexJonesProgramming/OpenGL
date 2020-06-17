#pragma once
#include <string>
#include <vector>
#include <math3d.h>
#include <glfuncs.h>
using namespace std;


class Model
{
public:

	string filename;
	float mXpos;
	float mYpos;
	float mZpos;
	float mXscale;
	float mYscale;
	float mZscale;
	float mXrot;
	float mYrot;
	float mZrot;

	std::vector< vec3 > vertices;
	std::vector< vec2 > uvs;
	std::vector< vec3 > normals;
	GLuint vao;
	

	Model(const char * filename, float x, float y, float z)
	{
		//make another function to load the file here
		this->mXpos = x;
		this->mYpos = y;
		this->mZpos = z;
		loadOBJ(filename, this->vertices, this->uvs, this->normals);

		// this sets up the VAO
		GLuint tmp[1];
		glGenVertexArrays(1, tmp);
		/*
		this->vao = tmp[0];

		glBindVertexArray(this->vao);


		//this adds the verts to the buffer 
		glGenBuffers(1, tmp);
		GLuint vbuff = tmp[0];
		glBindBuffer(GL_ARRAY_BUFFER, vbuff);
		glBufferData(GL_ARRAY_BUFFER, vertices .size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(Program::POSITION_INDEX);
		glVertexAttribPointer(Program::POSITION_INDEX, 3, GL_FLOAT, false, 3 * 4, 0);

		// adds the UV's to the buffer
		glGenBuffers(1, tmp);
		GLuint tbuff = tmp[0];
		glBindBuffer(GL_ARRAY_BUFFER, tbuff);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(Program::TEXCOORD_INDEX);
		glVertexAttribPointer(Program::TEXCOORD_INDEX, 2, GL_FLOAT, false, 2 * 4, 0);


		//adds the normals to the buffer
		glGenBuffers(1, tmp);
		GLuint nbuff = tmp[0];
		glBindBuffer(GL_ARRAY_BUFFER, nbuff);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(Program::NORMAL_INDEX);
		glVertexAttribPointer(Program::NORMAL_INDEX, 3, GL_FLOAT, false, 3 * 4, 0);


		glBindVertexArray(0);
		*/
		
	}



	~Model(){}



	void render()
	{
		glBindVertexArray(this->vao);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	}



	void loadOBJ(const char * path,
		std::vector < vec3 > & out_vertices,
		std::vector < vec2 > & out_uvs,
		std::vector < vec3 > & out_normals) {
		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
		std::vector< vec3 > temp_vertices;
		std::vector< vec2 > temp_uvs;
		std::vector< vec3 > temp_normals;

		FILE * file = fopen(path, "r");
		if (file == NULL) { printf("Impossible to open the file !\n"); }
		while (1) {

			char lineHeader[128];
			// read the first word of the line
			int res = fscanf(file, "%s", lineHeader);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

					   // else : parse lineHeader
			if (strcmp(lineHeader, "v") == 0) {
				vec3 vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);

			}
			else if (strcmp(lineHeader, "vt") == 0) {
				vec2 uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				temp_uvs.push_back(uv);

			}

			else if (strcmp(lineHeader, "vn") == 0) {
				vec3 normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);

			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], 
					&normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					std::string s = std::to_string(matches);
					char const *pchar = s.c_str();
					printf(pchar);
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					break;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);

				// For each vertex of each triangle
				for (unsigned int i = 0; i < vertexIndices.size(); i++) {
					unsigned int vertexIndex = vertexIndices[i];
					vec3 vertex = temp_vertices[vertexIndex - 1];
					out_vertices.push_back(vertex);
				}
				for (unsigned int i = 0; i < uvIndices.size(); i++) {
					unsigned int uvIndex = uvIndices[i];
					vec2 vertex = temp_uvs[uvIndex - 1];
					out_uvs.push_back(vertex);
				}
				for (unsigned int i = 0; i < normalIndices.size(); i++) {
					unsigned int normalIndex = normalIndices[i];
					vec3 vertex = temp_normals[normalIndex - 1];
					out_normals.push_back(vertex);
				}
			}
		}
	}



};

