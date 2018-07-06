// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesTetrahedralGrid3D_Header
#define DTSeriesTetrahedralGrid3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTTetrahedralGrid3D.h"

class DTTetrahedralGrid3D;
class DTDoubleArray;

class DTSeriesTetrahedralGrid3D : public DTSeries
{
public:
    DTSeriesTetrahedralGrid3D() : DTSeries("TetrahedralGrid3D") {}
    DTSeriesTetrahedralGrid3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"TetrahedralGrid3D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesTetrahedralGrid3D ByName(string);
    
    void Add(DTTetrahedralGrid3D v,double time);
    
    DTTetrahedralGrid3D Get(double time) const;
    DTTetrahedralGrid3D operator()(int index) const;

private:
    mutable DTTetrahedralGrid3D_SaveInfo saveInfo;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesTetrahedralGrid3D &toReturn);

#endif

