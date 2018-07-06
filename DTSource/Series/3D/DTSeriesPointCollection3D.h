// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPointCollection3D_Header
#define DTSeriesPointCollection3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPointCollection3D;

class DTSeriesPointCollection3D : public DTSeries
{
public:
    DTSeriesPointCollection3D() : DTSeries("PointCollection3D") {}
    DTSeriesPointCollection3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"PointCollection3D") {}

    void SetShouldSaveDoubleAsFloat(bool v=true) {_saveDoubleAsFloat = v;}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPointCollection3D ByName(string);
    
    void Add(DTPointCollection3D v,double time);

    DTPointCollection3D Get(double time) const;
    DTPointCollection3D operator()(int index) const;

private:
    bool _saveDoubleAsFloat;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPointCollection3D &toReturn);

#endif

