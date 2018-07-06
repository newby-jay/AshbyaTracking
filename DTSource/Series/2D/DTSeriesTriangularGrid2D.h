// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesTriangularGrid2D_Header
#define DTSeriesTriangularGrid2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTTriangularGrid2D.h"

class DTTriangularGrid2D;
class DTDoubleArray;

class DTSeriesTriangularGrid2D : public DTSeries
{
public:
    DTSeriesTriangularGrid2D() : DTSeries("TriangularGrid2D") {}
    DTSeriesTriangularGrid2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"TriangularGrid2D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesTriangularGrid2D ByName(string);
    
    void Add(DTTriangularGrid2D v,double time);
    
    DTTriangularGrid2D Get(double time) const;
    
private:
    mutable DTTriangularGrid2D_SaveInfo saveInfo;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesTriangularGrid2D &toReturn);

#endif

