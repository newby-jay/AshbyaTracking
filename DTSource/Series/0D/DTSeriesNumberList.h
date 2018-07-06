// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesNumberList_Header
#define DTSeriesNumberList_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTSeriesNumberList : public DTSeries
{
public:
    DTSeriesNumberList() : DTSeries("NumberList") {}
    DTSeriesNumberList(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"NumberList") {}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesNumberList ByName(string);
    
    void Add(const DTDoubleArray &v,double time);
    void Add(const DTFloatArray &v,double time);
    void Add(const DTIntArray &v,double time);
    DTDoubleArray Get(double time) const;
	DTIntArray GetIntArray(double time) const;

private:

};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesNumberList &toReturn);

#endif

