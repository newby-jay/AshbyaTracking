// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTVectorCollection3D_H
#define DTVectorCollection3D_H

#include "DTDoubleArray.h"
#include "DTDataStorage.h"
#include "DTPointCollection3D.h"

class DTVectorCollection3D {
public:
    DTVectorCollection3D() : points(), vectors() {}
    DTVectorCollection3D(const DTPointCollection3D &points,const DTDoubleArray &vectors);
    
    bool IsEmpty(void) const {return points.IsEmpty();}
    DTPointCollection3D Points(void) const {return points;}
    DTDoubleArray Vectors(void) const {return vectors;}
    
    void pinfo(void) const;
    
private:
    DTPointCollection3D points;
    DTDoubleArray vectors; // 3xN array.
};

extern DTRegion3D BoundingBox(const DTVectorCollection3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTVectorCollection3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTVectorCollection3D &variable);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTVectorCollection3D &variable);

extern void Read(const DTDataStorage &,const std::string &name,DTVectorCollection3D &toReturn,DTPointCollection3D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTVectorCollection3D &toWrite,DTPointCollection3D_SaveInfo &);

#endif
