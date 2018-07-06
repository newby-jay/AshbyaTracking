// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesMesh2DGrid_Header
#define DTSeriesMesh2DGrid_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTMesh2DGrid;

class DTSeriesMesh2DGrid : public DTSeries
{
public:
    DTSeriesMesh2DGrid() : DTSeries("Mesh2DGrid") {}
    DTSeriesMesh2DGrid(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Mesh2DGrid") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesMesh2DGrid ByName(string);
    
    void Add(DTMesh2DGrid v,double time);

    DTMesh2DGrid Get(double time) const;
    
private:
    
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesMesh2DGrid &toReturn);

#endif

