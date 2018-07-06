// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPath3DValues_H
#define DTPath3DValues_H

#include "DTPath3D.h"
#include "DTDoubleArray.h"
#include "DTDataStorage.h"

struct DTRegion1D;
struct DTRegion3D;
struct DTPoint3DValue;

// Layout is:
// If path is stored as
//    [ 0 x1 .... xN 0 x1 ... xM ...]
//    [ 0 y1 .... yN 0 y1 ... yM ...]
//    [ N z1 .... zN M z1 ... zM ...]
// Then the values list should be packed the same way:
//    [ N v1 .... vN M v1 ... vM ...]
// This allows multiple disconnected segments to be saved in a single array.
// This also allows, paths to be shared across path value objects, for example if you are
// resolving several scalars on the interface.

class DTPath3DValues {
public:
    DTPath3DValues() : path(), values() {};
    DTPath3DValues(const DTPath3D &path,const DTDoubleArray &values);
    
    bool IsEmpty(void) const {return path.IsEmpty();}
    DTPath3D Grid(void) const {return path;}
    DTPath3D Path(void) const {return path;}
    DTDoubleArray Values(void) const {return values;}
    
    int NumberOfPoints(void) const {return path.NumberOfPoints();}
    int NumberOfLoops(void) const {return path.NumberOfLoops();}
    
    void pinfo(void) const;
	void pall(void) const;
    
protected:
    DTPath3D path;
    DTDoubleArray values;
};

extern bool operator==(const DTPath3DValues &,const DTPath3DValues &);
extern bool operator!=(const DTPath3DValues &,const DTPath3DValues &);

extern DTPath3DValues operator-(const DTPath3DValues &,const DTPath3DValues &);

extern DTRegion3D BoundingBox(const DTPath3DValues &);
extern DTRegion1D ValueRange(const DTPath3DValues &);
extern double Minimum(const DTPath3DValues &);

extern DTPath3DValues ExtractLoop(const DTPath3DValues &P,int i);
extern DTPath3DValues Combine(const DTList<DTPath3DValues> &);
extern DTPoint3DValue FirstPoint(const DTPath3DValues &);
extern DTPoint3DValue LastPoint(const DTPath3DValues &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPath3DValues &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPath3DValues &thePath);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPath3DValues &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &,const std::string &name,DTPath3DValues &toReturn,DTPath3D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTPath3DValues &toWrite,DTPath3D_SaveInfo &);

class DTMutablePath3DValues : public DTPath3DValues
{
public:
	DTMutablePath3DValues() : DTPath3DValues() {}
	explicit DTMutablePath3DValues(const DTPath3D &); // Uninitialized
	
	DTMutablePath3DValues &operator=(double v);
	
private:
	DTMutableDoubleArray mutableValues;
};

#endif
