// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPoint2DValue_Header
#define DTSeriesPoint2DValue_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPoint2DValue;

class DTSeriesPoint2DValue : public DTSeries
{
public:
    DTSeriesPoint2DValue() : DTSeries("Point2DValue") {}
    DTSeriesPoint2DValue(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Point2DValue") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPoint2DValue ByName(string);
    
    void Add(DTPoint2DValue v,double time);

    DTPoint2DValue Get(double time) const;
    
private:
    
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPoint2DValue &toReturn);

#endif

