// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesTetrahedralMesh3D_Header
#define DTSeriesTetrahedralMesh3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTTetrahedralGrid3D.h"

class DTTetrahedralMesh3D;
class DTDoubleArray;

class DTSeriesTetrahedralMesh3D : public DTSeries
{
public:
    DTSeriesTetrahedralMesh3D() : DTSeries("TetrahedralMesh3D") {}
    DTSeriesTetrahedralMesh3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"TetrahedralMesh3D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesTetrahedralMesh3D ByName(string);
    
    void Add(DTTetrahedralMesh3D v,double time);
    
    DTTetrahedralMesh3D Get(double time) const;
    DTTetrahedralMesh3D operator()(int index) const;

private:
    mutable DTTetrahedralGrid3D_SaveInfo saveInfo;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesTetrahedralMesh3D &toReturn);

#endif

