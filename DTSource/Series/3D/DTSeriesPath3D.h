// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesPath3D_Header
#define DTSeriesPath3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTPath3D;

class DTSeriesPath3D : public DTSeries
{
public:
    DTSeriesPath3D() : DTSeries("Path3D"), _saveDoubleAsFloat(false) {}
    DTSeriesPath3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Path3D"), _saveDoubleAsFloat(true) {}
    
    void SetShouldSaveDoubleAsFloat(bool v=true) {_saveDoubleAsFloat = v;}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesPath3D ByName(string);
    
    void Add(DTPath3D v,double time);
    
    DTPath3D Get(double time) const;
    DTPath3D operator()(int index) const;
    
private:
    bool _saveDoubleAsFloat;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesPath3D &toReturn);

#endif
