#pragma once

#ifndef Arrow_H
#define Arrow_H

//#include <iostream>
//#include <fstream>
#include "Mesh.h"

class Arrow
{
public:
	Mesh* arrowMesh;
	Mesh* banner;
	vector<string> mData;
	float speed;
	float worldY;
	float worldSpin;
	int nextArrow;

	Arrow();

	void draw(Program* prog);

	bool checkArrow(string s);

	void update(float deltaTime);

	void importLevel(string fileName);

	void resetLevel();
};

#endif