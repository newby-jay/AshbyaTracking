// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesTetrahedralVectorField3D_Header
#define DTSeriesTetrahedralVectorField3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTTetrahedralGrid3D.h"

class DTTetrahedralVectorField3D;
class DTDoubleArray;

class DTSeriesTetrahedralVectorField3D : public DTSeries
{
public:
    DTSeriesTetrahedralVectorField3D() : DTSeries("TetrahedralVectorField3D") {}
    DTSeriesTetrahedralVectorField3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"TetrahedralVectorField3D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesTetrahedralVectorField3D ByName(string);
    
    void Add(DTTetrahedralVectorField3D v,double time);
    
    DTTetrahedralVectorField3D Get(double time) const;
    DTTetrahedralVectorField3D operator()(int index) const;

private:
    mutable DTTetrahedralGrid3D_SaveInfo saveInfo;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesTetrahedralVectorField3D &toReturn);

#endif

