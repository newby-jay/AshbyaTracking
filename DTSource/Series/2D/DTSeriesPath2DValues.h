// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPath2DValues_Header
#define DTSeriesPath2DValues_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPath2DValues;

class DTSeriesPath2DValues : public DTSeries
{
public:
    DTSeriesPath2DValues() : DTSeries("Path2DValues") {}
    DTSeriesPath2DValues(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Path2DValues") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPath2DValues ByName(string);
    
    void Add(DTPath2DValues v,double time);

    DTPath2DValues Get(double time) const;
    
private:
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPath2DValues &toReturn);

#endif

