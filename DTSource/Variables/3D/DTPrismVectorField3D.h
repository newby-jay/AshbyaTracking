// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPrismVectorField3D_H
#define DTPrismVectorField3D_H

#include "DTPrismGrid3D.h"
#include "DTDataStorage.h"

class DTPrismVectorField3D {
public:
    DTPrismVectorField3D()  {}
    DTPrismVectorField3D(const DTPrismGrid3D &,const DTFloatArray &,const DTFloatArray &,const DTFloatArray &);
    
    bool IsEmpty(void) const {return grid.IsEmpty();}
    DTPrismGrid3D Grid(void) const {return grid;}
    DTFloatArray X(void) const {return xComponent;}
    DTFloatArray Y(void) const {return yComponent;}
    DTFloatArray Z(void) const {return zComponent;}
    
    void pinfo(void) const;
    
private:
    DTPrismGrid3D grid;
    DTFloatArray xComponent;
    DTFloatArray yComponent;
    DTFloatArray zComponent;
};

extern DTRegion3D BoundingBox(const DTPrismVectorField3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPrismVectorField3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPrismVectorField3D &toWrite);

extern void Read(const DTDataStorage &,const std::string &name,DTPrismVectorField3D &toReturn,DTPrismGrid3D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTPrismVectorField3D &toWrite,DTPrismGrid3D_SaveInfo &);

#endif
