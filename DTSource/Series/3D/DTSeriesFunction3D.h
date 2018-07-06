// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesFunction3D_Header
#define DTSeriesFunction3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTFunction3D;

class DTSeriesFunction3D : public DTSeries
{
public:
    DTSeriesFunction3D() : DTSeries("Function3D") {}
    DTSeriesFunction3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Function3D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesFunction3D ByName(string);
    
    void Add(DTFunction3D v,double time);
    
    DTFunction3D Get(double time) const;
    DTFunction3D operator()(int index) const;

private:
        
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesFunction3D &toReturn);

#endif

