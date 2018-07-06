// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPath3D_H
#define DTPath3D_H

#include "DTDoubleArray.h"
#include "DTDataStorage.h"
#include "DTRegion3D.h"
#include "DTShiftScale3D.h"

// A polygon class.  The data array has one of two formats
// 3xN with a packed loop format.
// 6xN that saves every line segment.

// Layout is
// [ 0 x1 .... xN 0 x1 ... xM ...]
// [ 0 y1 .... yN 0 y1 ... yM ...]
// [ N z1 .... zN M z1 ... zM ...]
// This allows multiple loops to be saved in a single array.


/*
 A standard traversal code will look like:
 
 DTDoubleArray loops = path.Data();
 int loc = 0;
 int len = loops.n();
 int ptN,loopLength,loopStarts,loopEnds;
 while (loc<len) {
     loopLength = int(loops(2,loc));
     loopStarts = loc+1;
     loopEnds = loc+1+loopLength;
     for (ptN=loopStarts;ptN<loopEnds;ptN++) {
         // x = loops(0,ptN);
         // y = loops(1,ptN);
         // z = loops(2,ptN);
     }
     loc = loopEnds; // Prepare for the next loop.
 }
 */

class DTPath3D {
public:
    DTPath3D() : _cropBox(), _data() {};
    explicit DTPath3D(const DTDoubleArray &input);
    DTPath3D(const DTDoubleArray &input,DTRegion3D crop);

    bool IsEmpty() const {return _data.IsEmpty();}

    DTDoubleArray Data(void) const {return _data;}
    DTRegion3D CroppingBox(void) const {return _cropBox;}

    int NumberOfPoints(void) const;
    int NumberOfLoops(void) const;
    bool StoredAsSegments(void) const {return (_data.m()==6);}
	bool StoredAsLoops(void) const {return (_data.IsEmpty() || _data.m()==3);}

    void pinfo(void) const;
	void pall(void) const;
    
private:
    bool SetStorage(const DTDoubleArray &input);
    
    DTRegion3D _cropBox;
    DTDoubleArray _data;
    // 3xSomething or 6xSomething.  When read from a file it is always of the form 3xSomething
    // but you can save a 6xSomething array.
};

extern DTPath3D ExtractLoop(const DTPath3D &,int i);
extern DTPath3D SegmentForm(const DTPath3D &); // Free if already stored as segments.
extern DTPath3D LoopForm(const DTPath3D &);
extern DTPath3D Combine(const DTList<DTPath3D> &);
extern DTPath3D CombineLoops(const DTPath3D &,double eps); // Connects the loops, joins the point if closer than eps.
extern DTPath3D operator*(const DTShiftScale3D &,const DTPath3D &);
extern double Length(const DTPath3D &);

extern bool operator==(const DTPath3D &A,const DTPath3D &B);
extern bool operator!=(const DTPath3D &A,const DTPath3D &B);
extern DTPath3D operator+(const DTPath3D &A,const DTPath3D &B);

extern DTRegion3D BoundingBox(const DTPath3D &);
extern DTPoint3D FirstPoint(const DTPath3D &);
extern DTPoint3D LastPoint(const DTPath3D &);
extern DTMutableDoubleArray Points(const DTPath3D &); // Strips any connectivity.  Does not repeat endpoints in a closed loop.


// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPath3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPath3D &thePath);
extern void WriteFloat(DTDataStorage &output,const std::string &name,const DTPath3D &thePath);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPath3D &toWrite); // One time value, self documenting.

// A utility class to enable grid sharing.
struct DTPath3D_SaveInfo {
    DTPath3D path;
    std::string name;
};

// Pass in as the third argument to Read and Write to enable the the read/write routines
// to avoid having to read or write identical grids.  This is used when reading DTTrianglarMesh2D and DTTriangularVectorField2D
extern void Read(const DTDataStorage &input,const std::string &name,DTPath3D &toReturn,DTPath3D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTPath3D &toWrite,DTPath3D_SaveInfo &);

#endif
