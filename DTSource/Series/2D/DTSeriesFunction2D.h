// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesFunction2D_Header
#define DTSeriesFunction2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTFunction2D;

class DTSeriesFunction2D : public DTSeries
{
public:
    DTSeriesFunction2D() : DTSeries("Function2D") {}
    DTSeriesFunction2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Function2D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesFunction2D ByName(string);
    
    void Add(DTFunction2D v,double time);
    
    DTFunction2D Get(double time) const;
    
private:
        
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesFunction2D &toReturn);

#endif

