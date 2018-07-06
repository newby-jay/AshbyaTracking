// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTTriangularVectorField2D_H
#define DTTriangularVectorField2D_H

#include "DTDoubleArray.h"
#include "DTDataStorage.h"
#include "DTTriangularGrid2D.h"

class DTTriangularVectorField2D {
public:
    DTTriangularVectorField2D() : grid(), xComponent(), yComponent() {};
    DTTriangularVectorField2D(const DTTriangularGrid2D &grid,const DTDoubleArray &x,const DTDoubleArray &y);
    
    DTTriangularGrid2D Grid(void) const {return grid;}
    DTDoubleArray X(void) const {return xComponent;}
    DTDoubleArray Y(void) const {return yComponent;}
    
    int IsEmpty() const {return grid.IsEmpty();}
    int NotEmpty() const {return grid.NotEmpty();}
    
    void pinfo(void) const;
    
private:
    DTTriangularGrid2D grid;
    DTDoubleArray xComponent;
    DTDoubleArray yComponent;
};

bool operator==(const DTTriangularVectorField2D &,const DTTriangularVectorField2D &);
bool operator!=(const DTTriangularVectorField2D &,const DTTriangularVectorField2D &);

extern DTRegion2D BoundingBox(const DTTriangularVectorField2D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTTriangularVectorField2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTTriangularVectorField2D &variable);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTTriangularVectorField2D &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &,const std::string &name,DTTriangularVectorField2D &toReturn,DTTriangularGrid2D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTTriangularVectorField2D &toWrite,DTTriangularGrid2D_SaveInfo &);

#endif
