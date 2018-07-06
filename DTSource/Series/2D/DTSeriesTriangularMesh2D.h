// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesTriangularMesh2D_Header
#define DTSeriesTriangularMesh2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTTriangularGrid2D.h"

class DTTriangularMesh2D;
class DTDoubleArray;

class DTSeriesTriangularMesh2D : public DTSeries
{
public:
    DTSeriesTriangularMesh2D() : DTSeries("TriangularMesh2D") {}
    DTSeriesTriangularMesh2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"TriangularMesh2D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesTriangularMesh2D ByName(string);
    
    void Add(DTTriangularMesh2D v,double time);
    
    DTTriangularMesh2D Get(double time) const;
    
private:
    mutable DTTriangularGrid2D_SaveInfo saveInfo;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesTriangularMesh2D &toReturn);

#endif

