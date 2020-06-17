#pragma once
#include "math3d.h"
#include <string>
#include <fstream>
#include "Mesh.h"
using namespace std;

class Boxes
{
public:
	int numrows, numcols;
	vector<string> lines;
	vector<vector<int>> h;
	int height[1][1];
	Mesh* Box = new Mesh("Box.ms3d.mesh");

	Boxes(string filename) 
	{
		/*
		ifstream fp("map.txt");
		while (true) {
			string s;
			getline(fp, s);
			if (fp.fail())
				break;
			lines.push_back(s);
		}
		numrows = lines.size();
		numcols = lines[0].size();
		height = realloc();
		*/
		ifstream fp(filename);
		int x = 0;
		while (true) {
			string s;
			getline(fp, s);
			if (fp.fail())
				break;
			h.push_back(vector<int>());
			for (int i = 0;i < s.size();i++) {
				if (s.at(i) != ' ') {
					int temp = stoi(s.substr(i,1));
					h.at(x).push_back(temp);
				}
			}
			x++;
		}
		numrows = h.size();
		numcols = h[0].size();
		
	}


	void draw(Program* prog) 
	{	
		mat4 base = mat4::identity() * scaling(5, 5, 5)  * translation(vec3(numrows*-5, -50, numcols*-5 ));
		for (int r = 0; r< h.size(); ++r) {
			for (int c = 0; c<h[r].size(); ++c)
			{
				int temp = h.at(r).at(c);
				prog->setUniform("worldMatrix", base * translation(vec3(r * 10, temp * 10, c * 10)));
				this->Box->draw(prog);
			}
		}
	}
};