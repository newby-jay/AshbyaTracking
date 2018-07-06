// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPrismMesh3D_H
#define DTPrismMesh3D_H

#include "DTPrismGrid3D.h"
#include "DTDataStorage.h"

struct DTRegion1D;

class DTPrismMesh3D {
public:
    DTPrismMesh3D() : grid(), values() {}
    DTPrismMesh3D(const DTPrismGrid3D &Grid,const DTFloatArray &Values);
    
    bool IsEmpty(void) const {return grid.IsEmpty();}
    DTPrismGrid3D Grid(void) const {return grid;}
    DTFloatArray Values(void) const {return values;}
    
    void pinfo(void) const;
    
private:
    DTPrismGrid3D grid;
    DTFloatArray values;
};

extern DTRegion1D ValueRange(const DTPrismMesh3D &);
extern DTRegion3D BoundingBox(const DTPrismMesh3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPrismMesh3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPrismMesh3D &toWrite);

extern void Read(const DTDataStorage &,const std::string &name,DTPrismMesh3D &toReturn,DTPrismGrid3D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTPrismMesh3D &toWrite,DTPrismGrid3D_SaveInfo &);

#endif
