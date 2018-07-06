// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTVectorField2D_H
#define DTVectorField2D_H

#include "DTDoubleArray.h"
#include "DTDataStorage.h"
#include "DTMesh2DGrid.h"

class DTVectorField2D {
public:
    DTVectorField2D() : grid(), xComponent(), yComponent() {}
    DTVectorField2D(const DTMesh2DGrid grid,const DTDoubleArray &X,const DTDoubleArray &Y);

    ssize_t m(void) const {return grid.m();}
    ssize_t n(void) const {return grid.n();}
    
    DTMesh2DGrid Grid(void) const {return grid;}
    DTDoubleArray X(void) const {return xComponent;}
    DTDoubleArray Y(void) const {return yComponent;}
    
    void pinfo(void) const;

private:
    DTMesh2DGrid grid;
    DTDoubleArray xComponent;
    DTDoubleArray yComponent;
};

extern double MaximumLength(const DTVectorField2D &);
extern DTVectorField2D operator*(const DTVectorField2D &,double);

extern DTVectorField2D ApplyMask(const DTVectorField2D &field,const DTMask &mask);
extern DTVectorField2D RemoveMask(const DTVectorField2D &field);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTVectorField2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTVectorField2D &thePath);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTVectorField2D &toWrite); // One time value, self documenting.

#endif
