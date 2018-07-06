// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPointVector3D_Header
#define DTSeriesPointVector3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPointVector3D;

class DTSeriesPointVector3D : public DTSeries
{
public:
    DTSeriesPointVector3D() : DTSeries("Vector3D") {}
    DTSeriesPointVector3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Vector3D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPointVector3D ByName(string);
    
    void Add(DTPointVector3D v,double time);
    
    DTPointVector3D Get(double time) const;
    DTPointVector3D operator()(int index) const;

private:
        
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPointVector3D &toReturn);

#endif

