// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPoint3DValue_Header
#define DTSeriesPoint3DValue_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPoint3DValue;

class DTSeriesPoint3DValue : public DTSeries
{
public:
    DTSeriesPoint3DValue() : DTSeries("Point3DValue") {}
    DTSeriesPoint3DValue(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Point3DValue") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPoint3DValue ByName(string);
    
    void Add(DTPoint3DValue v,double time);

    DTPoint3DValue Get(double time) const;
    DTPoint3DValue operator()(int index) const;

private:
    
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPoint3DValue &toReturn);

#endif

