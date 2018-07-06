// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPath3DValues_Header
#define DTSeriesPath3DValues_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPath3DValues;

class DTSeriesPath3DValues : public DTSeries
{
public:
    DTSeriesPath3DValues() : DTSeries("Path3DValues") {}
    DTSeriesPath3DValues(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Path3DValues") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPath3DValues ByName(string);
    
    void Add(DTPath3DValues v,double time);

    DTPath3DValues Get(double time) const;
    DTPath3DValues operator()(int index) const;
    
private:
    
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPath3DValues &toReturn);

#endif

