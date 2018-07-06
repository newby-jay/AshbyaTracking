// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTVectorCollection2D_H
#define DTVectorCollection2D_H

#include "DTDoubleArray.h"
#include "DTDataStorage.h"
#include "DTPointCollection2D.h"

class DTVectorCollection2D {
public:
    DTVectorCollection2D() : points(), vectors() {}
    DTVectorCollection2D(const DTPointCollection2D &points,const DTDoubleArray &vectors);
    
    bool IsEmpty(void) const {return points.IsEmpty();}
    DTPointCollection2D Points(void) const {return points;}
    DTDoubleArray Vectors(void) const {return vectors;}
    
    void pinfo(void) const;
    
protected:
    DTPointCollection2D points;
    DTDoubleArray vectors; // 2xN array.
};

class DTMutableVectorCollection2D : public DTVectorCollection2D {
public:
    DTMutableVectorCollection2D() : DTVectorCollection2D() {}
    DTMutableVectorCollection2D(const DTPointCollection2D &,const DTMutableDoubleArray &);
    
    DTMutableDoubleArray Vectors(void) {return mutableVectors;}
    DTDoubleArray Vectors(void) const {return vectors;}
    
private:
    DTMutableDoubleArray mutableVectors;
};

extern DTRegion2D BoundingBox(const DTVectorCollection2D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTVectorCollection2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTVectorCollection2D &variable);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTVectorCollection2D &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &,const std::string &name,DTVectorCollection2D &toReturn,DTPointCollection2D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTVectorCollection2D &toWrite,DTPointCollection2D_SaveInfo &);

#endif
