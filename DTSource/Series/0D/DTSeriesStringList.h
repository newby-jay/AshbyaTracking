// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesStringList_Header
#define DTSeriesStringList_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTStringList;

class DTSeriesStringList : public DTSeries
{
public:
    DTSeriesStringList() : DTSeries("StringList") {}
    DTSeriesStringList(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"StringList") {}
    
    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesStringList ByName(string);
    
    void Add(DTStringList v,double time);
    
    DTStringList Get(double time) const;
    
private:
        
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesStringList &toReturn);

#endif

