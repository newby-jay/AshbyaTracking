// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPlot1D_Header
#define DTSeriesPlot1D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPlot1D;

class DTSeriesPlot1D : public DTSeries
{
public:
    DTSeriesPlot1D() : DTSeries("Plot1D") {}
    DTSeriesPlot1D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Plot1D") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPlot1D ByName(string);
    
    void Add(DTPlot1D v,double time);

    DTPlot1D Get(double time) const;

private:

};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPlot1D &toReturn);

#endif

