// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPointValueCollection3D_Header
#define DTSeriesPointValueCollection3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

#include "DTPointCollection3D.h"

class DTPointValueCollection3D;

class DTSeriesPointValueCollection3D : public DTSeries
{
public:
    DTSeriesPointValueCollection3D() : DTSeries("PointValueCollection3D"), _saveDoubleAsFloat(false) {}
    DTSeriesPointValueCollection3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"PointValueCollection3D"), _saveDoubleAsFloat(false) {}
    
    void SetShouldSaveDoubleAsFloat(bool v=true) {_saveDoubleAsFloat = v;}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPointValueCollection3D ByName(string);
    
    void Add(DTPointValueCollection3D v,double time);
    
    DTPointValueCollection3D Get(double time) const;
    DTPointValueCollection3D operator()(int index) const;

private:
    bool _saveDoubleAsFloat;
    mutable DTPointCollection3D_SaveInfo saveInfo;
};

extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPointValueCollection3D &toReturn);

#endif

