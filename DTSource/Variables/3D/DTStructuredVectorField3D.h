// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTStructuredVectorField3D_H
#define DTStructuredVectorField3D_H

#include "DTStructuredGrid3D.h"
#include "DTDataStorage.h"

class DTStructuredVectorField3D {
public:
    DTStructuredVectorField3D()  {}
    DTStructuredVectorField3D(const DTStructuredGrid3D &,const DTFloatArray &,const DTFloatArray &,const DTFloatArray &);
    
    bool IsEmpty(void) const {return grid.IsEmpty();}
    DTStructuredGrid3D Grid(void) const {return grid;}
    DTFloatArray X(void) const {return xComponent;}
    DTFloatArray Y(void) const {return yComponent;}
    DTFloatArray Z(void) const {return zComponent;}
    
    void pinfo(void) const;
    
private:
    DTStructuredGrid3D grid;
    DTFloatArray xComponent;
    DTFloatArray yComponent;
    DTFloatArray zComponent;
};

extern DTRegion3D BoundingBox(const DTStructuredVectorField3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTStructuredVectorField3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTStructuredVectorField3D &toWrite);

extern void Read(const DTDataStorage &,const std::string &name,DTStructuredVectorField3D &toReturn,DTStructuredGrid3D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTStructuredVectorField3D &toWrite,DTStructuredGrid3D_SaveInfo &);

#endif
