// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesMesh3DGrid_Header
#define DTSeriesMesh3DGrid_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTMesh3DGrid;

class DTSeriesMesh3DGrid : public DTSeries
{
public:
    DTSeriesMesh3DGrid() : DTSeries("Mesh3DGrid") {}
    DTSeriesMesh3DGrid(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Mesh3DGrid") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesMesh3DGrid ByName(string);
    
    void Add(DTMesh3DGrid v,double time);

    DTMesh3DGrid Get(double time) const;
    DTMesh3DGrid operator()(int index) const;

private:
    
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesMesh3DGrid &toReturn);

#endif

