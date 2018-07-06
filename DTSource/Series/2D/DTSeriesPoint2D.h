// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPoint2D_Header
#define DTSeriesPoint2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPoint2D;

class DTSeriesPoint2D : public DTSeries
{
public:
    DTSeriesPoint2D() : DTSeries("Point2D") {}
    DTSeriesPoint2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Point2D") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPoint2D ByName(string);
    
    void Add(DTPoint2D v,double time);

    DTPoint2D Get(double time) const;

private:
    
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPoint2D &toReturn);

#endif

