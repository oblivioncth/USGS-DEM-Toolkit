#include "Dem.h"
#include "Point3.h"

using namespace std;


DEM::DEM(string filename)
{
    ifstream input(filename);
    string linein,fullfile;
    int linecount = 0;
    if (input.is_open())
    {
        cout << "Reading file\n";
        while (getline(input, linein))
        {
            //should only be one line anywhos
            fullfile += linein;
        }
        cout << "Done reading file\n";
        readA(fullfile.substr(0, 1024));
        points = new Point3[sizey*sizex];//gonna need up to 18ish MB ram
        //find length of b record as it's not standardized
        int blen = BLengthFinder(fullfile.substr(1024, fullfile.length() - 1024));

        while ((linecount + 1) * blen + 1023 <=(int) fullfile.length())//for each b record
            //untested with C records
        {
            string temp = fullfile.substr(linecount * blen +1024, blen);
            readB(temp);
            linecount++;
        }
    }
    else {
        cout << "error opening file";
    }
}

DEM::~DEM()
{
}

float *DEM::getelevations()
{
    //if you only want z values
    float * elevations = new float[sizex*sizey];
    for (int i = 0; i < sizex*sizey; i++)
        elevations[i] = points[i].z;
    return elevations;
}

Point3 * DEM::getpoints()
{
    return points;
}

int DEM::getsizex()
{
    return sizex;
}

int DEM::getsizey()
{
    return sizey;
}

string DEM::getname()
{
    return locationname;
}

void DEM::readA(string a)// A record handler
{
    string temp;
    stringstream mainss;
    //file names, not really useful heres
    locationname = a.substr(0, 40);
    metadata = a.substr(40, 69);

    mainss << a.substr(528, 12);
    mainss >> resolutiontypexy>>resolutiontypez;
    //1= radians
    //2= feet
    //3= meters
    //4= arc seconds
    mainss.clear();
    mainss.str("");



    temp = a.substr(816, 36);
    //actually need to seperate these due to lack of space between
    mainss << temp.substr(0, 12);
    mainss >> resolution[0];
    mainss.clear();
    mainss.str("");
    mainss << temp.substr(12, 12);
    mainss >> resolution[1];
    mainss.clear();
    mainss.str("");
    mainss << temp.substr(24, 12);
    mainss >> resolution[2];
    mainss.clear();
    mainss.str("");

    //get matrix size
    mainss << a.substr(853, 12);
    mainss >> sizex >> sizey;
    mainss.clear();
    mainss.str("");
    if (sizex == 1)//yes, that is in the standard
        sizex = sizey;

    // get borders of quadrangle
    mainss << convert4tranf(a.substr(547,738));
    mainss >> swx >> swy >> nwx >> nwy >> nex >> ney >> sex >> sey;// kill me now
    mainss.clear();
    mainss.str("");

}

void DEM::readB(string b)
{


    string temp;
    temp = b.substr(1, 12);
    int index,column;// = stoi(numbuilder);
    float offset;
    stringstream test;
    test << temp;
    test >> index >>column;
    //our index starts at 0
    index--;
    column--;
    test.clear();
    test.str("");
    // vertical offset of record, often 0
    test << convert4tranf(b.substr(73, 24));
    test >> offset;
    test.clear();
    test.str("");
    test << b.substr(144, b.length()-145);
    int i;
    //float xloc = ((float)column) / (sizex - 1);
    while (test >> i)
    {
        points[column * sizey +index].z = (float)i*resolution[2]+offset;
        //cout << points[(column - 1) * sizey + (index - 1)].z<<endl;

        //float yloc =((float) index) / (sizey - 1);
        points[column * sizey + index].x = swx + resolution[0]*column;
            //(float)((swx*(1 - xloc) + sex*xloc)*(1 - yloc) + (nwx*(1 - xloc) + nex*xloc)*yloc);
        points[column * sizey + index].y = swy + resolution[1]*index;
            //(float)((swy*(1 - yloc) + nwy*yloc)*(1 - xloc) + (sey*(1 - yloc) + ney*yloc)*xloc);
        index++;

    }


}
string DEM::convert4tranf(string f)
{
    //c++ doesn't like the D from fortran
    int p;
    while ((p = f.find_first_of('D'))>0)
        f.replace(p, 1, "E");
    return f;
}

int DEM::BLengthFinder(string nota){
    int length = 1;//assume record B exists
    stringstream tester;
    float temp;
    do
    {

        tester.clear();
        tester.str("");
        tester << nota.substr((length * 1024) + 73, 24);
        tester >> temp;
        length++;

    } while (tester >> temp);//if there's more than one number it's still elevation data
    return(length - 1) * 1024;
}

DEM::bounds DEM::calculatebounds()
{
    bounds boundCheck;

    if(nwy > ney)
        boundCheck.fYMax = nwy;
    else
        boundCheck.fYMax = ney;
    if(swy < sey)
        boundCheck.fYMin = swy;
    else
        boundCheck.fYMin = sey;

    if(nex > sex)
        boundCheck.fXMax = nex;
    else
        boundCheck.fXMax = sex;
    if(nwx < swx)
        boundCheck.fXMin = nwx;
    else
        boundCheck.fXMin = swx;

    return boundCheck;
}

