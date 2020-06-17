#pragma once
#include "math3d.h"
#include "Mesh.h"

class Rings
{
public:

	vec3 positions[15];
	float rotation = 0.0f;
	vec3 textPos = vec3(0,0,0);
	float textRot = 0.0f;
	int currentRing = 0;
	Mesh* ring = new Mesh("ring.blend.ms3d.mesh");
	Mesh* text = new Mesh("winText.blend.ms3d.mesh");
	Mesh* arrow = new Mesh("arrow.obj.mesh");
	float arrowXrot = 0.0f;
	float arrowYrot = 0.0f;
	

	Rings() 
	{
		positions[0] = vec3(-25,12,-222);
		positions[1] = vec3(-182, 85, -106);
		positions[2] = vec3(-275, 113, 22);
		positions[3] = vec3(-356, 217, 286);
		positions[4] = vec3(10, 275, -37);
		positions[5] = vec3(125, 133, -55);
		positions[6] = vec3(130, 76, 360);
		positions[7] = vec3(135, 12, 127);
		positions[8] = vec3(140, 98, -483);
		positions[9] = vec3(145, 220, 72);
		positions[10] = vec3(150, 25, 99);
		positions[11] = vec3(155, 36, -99);
		positions[12] = vec3(160, 154, 620);
		positions[13] = vec3(165, 88, -143);
		positions[14] = vec3(170, 500, 250);

	}

	void update(Helicopter * heli, float deltaTime)
	{
		if (currentRing <= 14)
		{
			// this is ring hit detection
			if (distance(positions[currentRing], heli->pos) < 5)
				currentRing++;
			//this points the arrow in the right direction
			arrowYrot = atan2(heli->pos.z - positions[currentRing].z, positions[currentRing].x - heli->pos.x) * 180 / 3.1415;
			if (arrowYrot < -90)
				arrowYrot += 180;
			if (arrowYrot > 90)
				arrowYrot -= 180;
			//cout << arrowYrot << "\n";
			arrowXrot = atan2(heli->pos.y - positions[currentRing].y, positions[currentRing].x - heli->pos.x) * 180 / 3.1415;

			// this rotates the rings
			rotation += 360 * deltaTime / 1000;
			if (rotation > 360)
				rotation -= 360;
		}
		textPos = heli->pos;
		textRot = heli->Yrot;
	}

	void draw(Program* prog)
	{
		for (int i = currentRing; i <= 14; i++)
		{
			prog->setUniform("worldMatrix", mat4::identity()
				* scaling(5, 5, 5)
				* axisRotation(vec3(0, 1, 0), rotation*(3.14159265359 / 180))
				* translation(positions[i]));
			ring->draw(prog);
		}
		prog->setUniform("doShadow", 0);
		if (currentRing > 14)
		{
			prog->setUniform("worldMatrix", mat4::identity()
				* axisRotation(vec3(0, 1, 0), (textRot+180)*(3.14159265359 / 180))
				* translation(vec3(0,2,0)) 
				* translation(textPos));
			text->draw(prog);
		}
		else
		{
			prog->setUniform("worldMatrix", mat4::identity()
				* scaling(0.5, 0.5, 0.5)
				* axisRotation(vec3(0, 0, 1), -arrowXrot*(3.14159265359 / 180))
				* axisRotation(vec3(0, 1, 0), arrowYrot*(3.14159265359 / 180))
				* translation(vec3(0, 4, 0))
				* translation(textPos));// text pos is also the heli pos
			arrow->draw(prog);
		}
		prog->setUniform("doShadow", 1);
	}

	float distance(vec3 pos1, vec3 pos2)
	{
		float x = (pos2.x - pos1.x) * (pos2.x - pos1.x);
		float y = (pos2.y - pos1.y) * (pos2.y - pos1.y);
		float z = (pos2.z - pos1.z) * (pos2.z - pos1.z);
		float r = sqrt(x + y + z);
		return r;
	}

};