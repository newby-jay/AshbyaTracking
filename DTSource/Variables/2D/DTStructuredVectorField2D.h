// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTStructuredVectorField2D_H
#define DTStructuredVectorField2D_H

#include "DTStructuredGrid2D.h"
#include "DTDataStorage.h"

class DTStructuredVectorField2D {
public:
    DTStructuredVectorField2D()  {}
    DTStructuredVectorField2D(const DTStructuredGrid2D &,const DTFloatArray &,const DTFloatArray &);
    
    bool IsEmpty(void) const {return grid.IsEmpty();}
    DTStructuredGrid2D Grid(void) const {return grid;}
    DTFloatArray X(void) const {return xComponent;}
    DTFloatArray Y(void) const {return yComponent;}
    
    void pinfo(void) const;
    
private:
    DTStructuredGrid2D grid;
    DTFloatArray xComponent;
    DTFloatArray yComponent;
};

extern DTRegion2D BoundingBox(const DTStructuredVectorField2D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTStructuredVectorField2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTStructuredVectorField2D &toWrite);

extern void Read(const DTDataStorage &,const std::string &name,DTStructuredVectorField2D &toReturn,DTStructuredGrid2D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTStructuredVectorField2D &toWrite,DTStructuredGrid2D_SaveInfo &);

#endif
