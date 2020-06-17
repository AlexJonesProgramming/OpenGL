#pragma once
#include "math3d.h"

class Helicopter
{
public:
	vec3 pos = vec3(-30,12,-300);
	float Xrot = 0.0f, Yrot = 0.0f, Zrot = 0.0f;
	int speed = 0;
	int gain = 0;
	int camAngle = 1.5;
	int frame = 30;
	Mesh* mesh = new Mesh("heliobird3.blend.ms3d.mesh");

	Helicopter() {}

	void update(float deltaTime)
	{
		//this is for the animation
		frame += 60 * deltaTime / 1000;
		if (frame >= 90)
			frame -= 60;

		// this handles all the movement
		pos.y += gain * (deltaTime / 1000);
		pos.x -= speed * cos(Yrot*(3.14159265359 / 180)) * (deltaTime / 1000);
		pos.z += speed * sin(Yrot*(3.14159265359 / 180)) * (deltaTime / 1000);
		
		if (gain > 10)
		{
			gain = 10;
		}
		if (gain < -50)
		{
			gain = -50;
		}
		if (speed > 50)
		{
			speed = 50;
		}
		if (speed < 0)
		{
			speed = 0;
		}
		if (Xrot > 360)
			Xrot -= 360;
		if (Yrot > 360)
			Yrot -= 360;
		if (Zrot > 360)
			Zrot -= 360;
		if (camAngle > 5)
			camAngle = 5;
		if (camAngle < -5)
			camAngle = -5;
	}

	void changeSpeed(int direction) 
	{
		if (direction == 1 || direction == -1)
			speed += direction;
		else
			printf("Change speed only works with 1 and -1.");
	}

	void changeGain(int direction)
	{
		if (direction == 1 || direction == -1 )
			gain += direction;
		else 
			printf("Change gain only works with 1 and -1.");
	}

	void turn(int direction, float deltaTime)
	{
		if (direction == 1 || direction == -1)
			Yrot += (direction * deltaTime / 1000) * 50;
		else
			printf("Change gain only works with 1 and -1.");
	}

	void draw(Program* prog) 
	{
		prog->setUniform("frame", frame);
		prog->setUniform("worldMatrix", mat4::identity() 
			* axisRotation(vec3(1, 0, 0), Xrot*(3.14159265359 / 180))
			* axisRotation(vec3(0, 1, 0), Yrot*(3.14159265359 / 180))
			* axisRotation(vec3(0, 0, 1), Zrot*(3.14159265359 / 180)) 
			* translation(pos));
		mesh->draw(prog);
	}

	void changeCamAngle(int direction)
	{
		if (direction == 1 || direction == -1)
			camAngle += direction;
		else
			printf("Change gain only works with 1 and -1.");
	}

	vec3 getCamPos()
	{
		//pos.x -= speed * cos(Yrot*(3.14159265359 / 180));
		//pos.z += speed * sin(Yrot*(3.14159265359 / 180));
		vec3 temp = vec3(15 * cos(Yrot*(3.14159265359 / 180)), camAngle, -15 * sin(Yrot*(3.14159265359 / 180)));
		vec3 temp2 = vec3(pos.x, pos.y, pos.z);
		temp = temp + temp2;

		return temp;
	}

	void printPos()
	{
		cout << pos.x << " " << pos.y << " " << pos.z << "\n" << "\n";
	}
};

