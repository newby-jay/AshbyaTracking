// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesVectorCollection2D_Header
#define DTSeriesVectorCollection2D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTVectorCollection2D;

class DTSeriesVectorCollection2D : public DTSeries
{
public:
    DTSeriesVectorCollection2D() : DTSeries("VectorCollection2D") {}
    DTSeriesVectorCollection2D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"VectorCollection2D") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesVectorCollection2D ByName(string);
    
    void Add(DTVectorCollection2D v,double time);

    DTVectorCollection2D Get(double time) const;
    
private:

};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesVectorCollection2D &toReturn);

#endif

