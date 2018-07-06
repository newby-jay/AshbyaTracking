// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesStructuredMesh2D_Header
#define DTSeriesStructuredMesh2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTStructuredGrid2D.h"

class DTStructuredMesh2D;
class DTDoubleArray;

class DTSeriesStructuredMesh2D : public DTSeries
{
public:
    DTSeriesStructuredMesh2D() : DTSeries("StructuredMesh2D"), lastGrid(), nameForLastGrid() {}
    DTSeriesStructuredMesh2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"StructuredMesh2D"), lastGrid(), nameForLastGrid() {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesStructuredMesh2D ByName(string);
    
    void Add(DTStructuredMesh2D v,double time);

    DTStructuredMesh2D Get(double time) const;

private:
    DTStructuredGrid2D lastGrid;
    std::string nameForLastGrid;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesStructuredMesh2D &toReturn);

#endif

