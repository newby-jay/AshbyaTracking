// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPointValueCollection3D_H
#define DTPointValueCollection3D_H

#include "DTDoubleArray.h"
#include "DTPointCollection3D.h"

#include <string>

class DTDataStorage;
struct DTRegion1D;
struct DTRegion3D;

class DTPointValueCollection3D {
public:
    DTPointValueCollection3D() : points(), values() {}
    DTPointValueCollection3D(const DTPointCollection3D &ptsIn,const DTDoubleArray &valIn);

    bool IsEmpty(void) const {return points.IsEmpty();}
    size_t NumberOfPoints(void) const {return points.NumberOfPoints();}
    DTPointCollection3D Points(void) const {return points;}
    DTDoubleArray Values(void) const {return values;}

    void pinfo(void) const;
	void pall(void) const;

private:
    DTPointCollection3D points;
    DTDoubleArray values;
};

extern DTPointValueCollection3D ConvertToDouble(const DTPointValueCollection3D &);

extern DTRegion3D BoundingBox(const DTPointValueCollection3D &);
extern DTRegion1D ValueRange(const DTPointValueCollection3D &);

extern DTPointValueCollection3D ExtractPoints(const DTPointValueCollection3D &,const DTIntArray &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPointValueCollection3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPointValueCollection3D &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPointValueCollection3D &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &,const std::string &name,DTPointValueCollection3D &toReturn,DTPointCollection3D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTPointValueCollection3D &toWrite,DTPointCollection3D_SaveInfo &);
extern void WriteFloat(DTDataStorage &,const std::string &name,const DTPointValueCollection3D &toWrite,DTPointCollection3D_SaveInfo &);

#endif
