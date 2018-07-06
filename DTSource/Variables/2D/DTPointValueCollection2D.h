// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPointValueCollection2D_H
#define DTPointValueCollection2D_H

#include "DTDoubleArray.h"
#include "DTPointCollection2D.h"
#include "DTPoint2DValue.h"

#include <string>

class DTDataStorage;
struct DTRegion1D;
struct DTRegion2D;

class DTPointValueCollection2D {
public:
    DTPointValueCollection2D() : points(), values() {}
    DTPointValueCollection2D(const DTPointCollection2D &ptsIn,const DTDoubleArray &valIn);
    
    bool IsEmpty(void) const {return points.IsEmpty();}
    size_t NumberOfPoints(void) const {return points.NumberOfPoints();}
    DTPointCollection2D Grid(void) const {return points;}
    DTPointCollection2D Points(void) const {return points;}
    DTDoubleArray Values(void) const {return values;}
    DTPoint2DValue operator()(int) const;

    void pinfo(void) const;
    void pall(void) const;
    
private:
    DTPointCollection2D points;
    DTDoubleArray values;
};

extern bool operator==(const DTPointValueCollection2D &,const DTPointValueCollection2D &);
extern bool operator!=(const DTPointValueCollection2D &,const DTPointValueCollection2D &);

extern DTRegion2D BoundingBox(const DTPointValueCollection2D &);
extern DTRegion1D ValueRange(const DTPointValueCollection2D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPointValueCollection2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPointValueCollection2D &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPointValueCollection2D &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &,const std::string &name,DTPointValueCollection2D &toReturn,DTPointCollection2D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTPointValueCollection2D &toWrite,DTPointCollection2D_SaveInfo &);

#endif
