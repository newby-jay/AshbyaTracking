// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTVectorField3D_H
#define DTVectorField3D_H

#include "DTFloatArray.h"
#include "DTDataStorage.h"
#include "DTMesh3DGrid.h"

class DTVectorField3D {
public:
    DTVectorField3D() : grid(), xComponent(), yComponent(), zComponent() {}
    DTVectorField3D(const DTMesh3DGrid &grid,const DTFloatArray &X,const DTFloatArray &Y,const DTFloatArray &Z);

    bool IsEmpty(void) const {return xComponent.IsEmpty();}
	
	ssize_t m(void) const {return grid.m();}
	ssize_t n(void) const {return grid.n();}
	ssize_t o(void) const {return grid.o();}
    
    DTMesh3DGrid Grid(void) const {return grid;}
    DTFloatArray X(void) const {return xComponent;}
    DTFloatArray Y(void) const {return yComponent;}
    DTFloatArray Z(void) const {return zComponent;}
    
    void pinfo(void) const;
    
private:
    DTMesh3DGrid grid;
    DTFloatArray xComponent;
    DTFloatArray yComponent;
    DTFloatArray zComponent;
};

extern DTVectorField3D ApplyMask(const DTVectorField3D &,const DTMask &);
extern DTVectorField3D RemoveMask(const DTVectorField3D &);

extern DTRegion3D BoundingBox(const DTVectorField3D &);
extern float MaximumLength(const DTVectorField3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTVectorField3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTVectorField3D &thePath);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTVectorField3D &toWrite); // One time value, self documenting.

#endif
