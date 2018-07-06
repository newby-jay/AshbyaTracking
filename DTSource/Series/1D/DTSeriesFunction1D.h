// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesFunction1D_Header
#define DTSeriesFunction1D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTFunction1D;

class DTSeriesFunction1D : public DTSeries
{
public:
    DTSeriesFunction1D() : DTSeries("Function1D") {}
    DTSeriesFunction1D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Function1D") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesFunction1D ByName(string);
    
    void Add(DTFunction1D v,double time);
    
    DTFunction1D Get(double time) const;
    
private:
        
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesFunction1D &toReturn);

#endif

