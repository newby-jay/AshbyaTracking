// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPointValueCollection2D_Header
#define DTSeriesPointValueCollection2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"
#include "DTPointCollection2D.h"

class DTPointValueCollection2D;

class DTSeriesPointValueCollection2D : public DTSeries
{
public:
    DTSeriesPointValueCollection2D() : DTSeries("PointValueCollection2D") {}
    DTSeriesPointValueCollection2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"PointValueCollection2D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPointValueCollection2D ByName(string);
    
    void Add(DTPointValueCollection2D v,double time);
    
    DTPointValueCollection2D Get(double time) const;
    DTPointValueCollection2D operator()(int index) const;
   
private:
    mutable DTPointCollection2D_SaveInfo saveInfo;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPointValueCollection2D &toReturn);

#endif
