// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPath2DValues_H
#define DTPath2DValues_H

#include "DTPath2D.h"
#include "DTDoubleArray.h"
#include "DTDataStorage.h"

struct DTRegion1D;
struct DTRegion2D;

// Layout is:
// If path is stored as
//    [ 0 x1 .... xN 0 x1 ... xM ...]
//    [ N y1 .... yN M y1 ... yM ...]
// Then the values list should be packed the same way:
//    [ N v1 .... vN M v1 ... vM ...]
// This allows multiple disconnected segments to be saved in a single array.
// This also allows paths to be shared across path value objects, for example if you are
// resolving several scalars on the interface.

class DTPath2DValues {
public:
    DTPath2DValues() : path(), values() {};
    DTPath2DValues(const DTPath2D &path,const DTDoubleArray &values);

    bool IsEmpty(void) const {return path.IsEmpty();}
    DTPath2D Grid(void) const {return path;}
    DTPath2D Path(void) const {return path;}
    DTDoubleArray Values(void) const {return values;}

    int NumberOfPoints(void) const {return path.NumberOfPoints();}
    int NumberOfLoops(void) const {return path.NumberOfLoops();}
    
    void pinfo(void) const;
    void pall(void) const;
	void ppath(void) const; // print the path only
	
private:
    DTPath2D path;
    DTDoubleArray values;
};

extern bool operator==(const DTPath2DValues &,const DTPath2DValues &);

extern DTPath2DValues operator-(const DTPath2DValues &,const DTPath2DValues &);

extern DTRegion2D BoundingBox(const DTPath2DValues &);
extern DTRegion1D ValueRange(const DTPath2DValues &);

extern double Maximum(const DTPath2DValues &);

extern DTMutableDoubleArray Points(const DTPath2DValues &); // Strips any connectivity.  Does not repeat endpoints in a closed loop.
extern DTMutableDoubleArray Values(const DTPath2DValues &);
extern DTPath2DValues ExtractLoop(const DTPath2DValues &P,ssize_t loop);

extern DTPath2DValues ChangeOrientation(const DTPath2DValues &);
extern DTPath2DValues Combine(const DTList<DTPath2DValues> &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPath2DValues &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPath2DValues &thePath);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPath2DValues &toWrite); // One time value, self documenting.

extern void Read(const DTDataStorage &,const std::string &name,DTPath2DValues &toReturn,DTPath2D_SaveInfo &);
extern void Write(DTDataStorage &,const std::string &name,const DTPath2DValues &toWrite,DTPath2D_SaveInfo &);

#endif
