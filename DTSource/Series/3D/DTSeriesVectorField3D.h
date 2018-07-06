// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesVectorField3D_Header
#define DTSeriesVectorField3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTVectorField3D.h"

class DTDoubleArray;

class DTSeriesVectorField3D : public DTSeries
{
public:
    DTSeriesVectorField3D() : DTSeries("VectorField3D"), t1(-1.0), t2(-1.0), k1(-1), k2(-1) {}
    DTSeriesVectorField3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"VectorField3D"), t1(-1.0), t2(-1.0), k1(-1), k2(-1) {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesVectorField3D ByName(string);
    
    void Add(DTVectorField3D v,double time);
    
    DTVectorField3D Get(double time) const;
    DTVectorField3D GetInterpolated(double time) const;
    DTVectorField3D operator()(int index) const;

private:
    mutable DTVectorField3D v1,v2;
    mutable double t1,t2;
    mutable int k1,k2;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesVectorField3D &toReturn);

#endif

