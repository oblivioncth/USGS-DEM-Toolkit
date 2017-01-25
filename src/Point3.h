#pragma once

#ifndef POINT3_H
#define POINT3_H

class Point3 {
public:
	Point3();
	Point3(float, float, float);
	~Point3();

	//private: fuck getters and setters
	float x, y, z;
};

#endif POINT3_H