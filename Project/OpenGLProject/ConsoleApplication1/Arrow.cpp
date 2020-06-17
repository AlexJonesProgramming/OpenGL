#pragma once
#include "Arrow.h"
#include "math3d.h"

Arrow::Arrow()
{
	arrowMesh = new Mesh("arrow.obj.mesh");

	banner = new Mesh("ArrowHitBox.obj.mesh");
	speed = 0.0;
	worldY = -10;
	worldSpin = 0;
	nextArrow = 0;
}

void Arrow::draw(Program* prog)
{
	/*
	Arrow* testArrow1 = new Arrow(vec3(-3.75, 0, -25), 180); // left
	Arrow* testArrow2 = new Arrow(vec3(-1.25, 0, -25), -90); // down
	Arrow* testArrow3 = new Arrow(vec3(1.25, 0, -25), 90); // up
	Arrow* testArrow4 = new Arrow(vec3(3.75, 0, -25), 0); // right
	*/
	// left = axisRotation(vec3(0,1,0), 180)
	// right = axisRotation(vec3(0,1,0), 180)
	// up = axisRotation(vec3(0,0,1), 90)
	// down = axisRotation(vec3(0,0,1), -90)

	vec3 curVec = vec3();
	float curRot = 0.0;
	int spinMode = 0; // 0 is around the y axis 1 is around the x axis
	bool draw = true;

	for (int i = 0; i < mData.size(); i++)
	{
		if (mData[i] == "left")
		{
			curRot = 180;
			curVec.x = -3.75;
			spinMode = 1;
		}
		if (mData[i] == "down")
		{
			curRot = -90;
			curVec.x = -1.25;
			spinMode = 0;
		}
		if (mData[i] == "up")
		{
			curRot = 90;
			curVec.x = 1.25;
			spinMode = 0;
		}
		if (mData[i] == "right")
		{
			curRot = 0;
			curVec.x = 3.75;
			spinMode = 1;
		}

		curVec.y = i * -2 + worldY;
		if (i > nextArrow && curVec.y > 10) { nextArrow++; } // this should work? it's used for the input testing.
		if (curVec.y > -13 && curVec.y < 10 && mData[i] != "blank") { draw = true; }
		else { draw = false; }
		curVec.z = -25.0;

		if (draw && spinMode == 1)
		{
			prog->setUniform("worldMatrix", mat4::identity()
				* axisRotation(vec3(0, 0, 1), curRot*(3.14159265359 / 180))
				* axisRotation(vec3(1, 0, 0), worldSpin*(3.14159265359 / 180))
				* translation(curVec));
			this->arrowMesh->draw(prog);
		}
		else if (draw && spinMode == 0)
		{
			prog->setUniform("worldMatrix", mat4::identity()
				* axisRotation(vec3(0, 0, 1), curRot*(3.14159265359 / 180))
				* axisRotation(vec3(0, 1, 0), worldSpin*(3.14159265359 / 180))
				* translation(curVec));
			this->arrowMesh->draw(prog);
		}
		prog->setUniform("worldMatrix", mat4::identity() * translation(vec3(-0.6, 9.5, -23)));

	}banner->draw(prog);
}


bool Arrow::checkArrow(string s)
{
	if (s.compare("right"))  // .compare() returns 0 if it is the same, and 1 if it flase (0 is the same as false to an if statement, 1 is true)
	{						 // so this gets triggered if s is not "right" 
		if (s.compare("left"))
		{
			if (s.compare("up"))
			{
				if (s.compare("down"))
				{
					cout << "checkArrow isn't working properly, try using right, left, up, or down next time.\n";
					return false;
				}
			}
		}
	}

	string testString = mData[nextArrow];
	float testHight = nextArrow * -2 + worldY;
	if (testHight < 9 || testHight > 11) { return false; }
	else if (testString.compare(s) == 0) { return true; }
	else { return false; }

}

void Arrow::update(float deltaTime)
{
	worldY += (speed * deltaTime);
	worldSpin += (0.36 * deltaTime);
}

void Arrow::importLevel(string fileName)
{
	/*
	this is how we will set up the text files to load in the levels

	the first line will be the speed of the level which will tell us how far to place the objects appart from one another
	we could have the speed set as (slow, medium, fast, ultimate)
	rotation (left, right, up, down) or blank for skipped sections

	*/

	ifstream text(fileName);
	if (!text)
		throw runtime_error("Cannot load file for import | Arrow->importLevel()");

	mData.clear();
	resetLevel();
	string s;

	while (getline(text, s))
	{

		if (s.compare("slow") == 0)
			speed = 0.005;
		else if (s.compare("medium") == 0)
			speed = 0.01;
		else if (s.compare("fast") == 0)
			speed = 0.015;
		else if (s.compare("ultimate") == 0)
			speed = 0.02;
		else { mData.push_back(s); }

		//cout << s << "\n";		
	}
	text.close();

}

void Arrow::resetLevel()
{
	worldY = -10;
	worldSpin = 0;
	nextArrow = 0;
}