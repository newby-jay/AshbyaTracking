// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesSurface3D_Header
#define DTSeriesSurface3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTSurface3D;

class DTSeriesSurface3D : public DTSeries
{
public:
    DTSeriesSurface3D() : DTSeries("Surface3D"), _saveDoubleAsFloat(false) {}
    DTSeriesSurface3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Surface3D"), _saveDoubleAsFloat(false) {}

    void SetShouldSaveDoubleAsFloat(bool v=true) {_saveDoubleAsFloat = v;}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesSurface3D ByName(string);
    
    void Add(DTSurface3D v,double time);

    DTSurface3D Get(double time) const;
    DTSurface3D operator()(int index) const;

private:
    bool _saveDoubleAsFloat;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesSurface3D &toReturn);

#endif

