#pragma once

#ifndef DEM_H
#define DEM_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Point3.h"

using namespace std;

class DEM
{
public:

    struct bounds
    {
        float fXMin;
        float fXMax;
        float fYMin;
        float fYMax;
    };

	DEM(string);
	~DEM();
    float *getelevations();
	Point3* getpoints();
    int getsizex();
    int getsizey();
    bounds calculatebounds();
    string getname();
	

private:
	void readA(string);
	void readB(string);
	string convert4tranf(string);
    int BLengthFinder(string nota);
	string locationname;
	float resolution[3];
	int resolutiontypexy;
	int resolutiontypez;
	int units;
	int zone;
	double swx, nwx, nex, sex;
	double swy, nwy, ney, sey;
	int sizex, sizey;
	Point3* points;

	string metadata;
};

#endif // DEM_H

