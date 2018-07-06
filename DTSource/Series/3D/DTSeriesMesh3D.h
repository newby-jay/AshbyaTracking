// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTSeriesMesh3D_Header
#define DTSeriesMesh3D_Header

// See DTSeries.h for further information.
#include "DTSeries.h"

class DTMesh3D;
class DTMesh3DGrid;
class DTDoubleArray;
class DTMesh3DBanded;

class DTSeriesMesh3D : public DTSeries
{
public:
    DTSeriesMesh3D() : DTSeries("Mesh3D"), _saveDoubleAsFloat(false) {}
    DTSeriesMesh3D(DTDataStorage &saveInto,string name) : DTSeries(saveInto,name,"Mesh3D"), _saveDoubleAsFloat(false) {}

    void SetShouldSaveDoubleAsFloat(bool v=true) {_saveDoubleAsFloat = v;}

    void Register() const;
    void Register(string) const;
    static void pregistered(void); // Debug call, lists all registered series of this type.
    static bool HasBeenRegistered(string);
    static DTSeriesMesh3D ByName(string);
    
    void Add(const DTDoubleArray &v,double time);
    void Add(const DTDoubleArray &v,const DTMesh3DGrid &grid,double time);
    void Add(const DTMesh3D &v,double time);
    void Add(const DTMesh3DBanded &v,double time);
    
    DTMesh3D Get(double time) const;
    DTMesh3D operator()(int index) const;

private:
    bool _saveDoubleAsFloat;
};

// To read series by reference. 
extern void Read(const DTDataStorage &input,const std::string &name,DTSeriesMesh3D &toReturn);

#endif

