// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPath2D_H
#define DTPath2D_H

class DTDataStorage;

#include "DTDoubleArray.h"
#include "DTRegion2D.h"

class DTIntArray;
class DTMutableIntArray;

// A polygon class.  The data array has one of two formats
// 2xN with a packed loop format.
// 4xN that saves every line segment.

// Layout is
// [ 0 x1 .... xN 0 x1 ... xM ...]
// [ N y1 .... yN M y1 ... yM ...]
// This allows multiple loops to be saved in a single array.

// Look at the BoundingBox() function for an example code that parses the structure.

// There is also a cropping region.  The main use of this is to
// be able to determine if a path ending is due to the cropping.

/*
 A standard traversal code will look like:
 
 DTDoubleArray loops = path.Data();
 int loc = 0;
 int len = loops.n();
 int ptN,loopLength,loopStarts,loopEnds;
 while (loc<len) {
     loopLength = int(loops(1,loc));
     loopStarts = loc+1;
     loopEnds = loc+1+loopLength;
     // loopClosed = (loops(0,loopStarts)==loops(0,loopEnds-1) && loops(1,loopStarts)==loops(1,loopEnds-1));
     for (ptN=loopStarts;ptN<loopEnds;ptN++) {
         // x = loops(0,ptN);
         // y = loops(1,ptN);
     }
     loc = loopEnds; // Prepare for the next loop.
 }
 */

class DTPath2D {
public:
    DTPath2D() : _cropBox(), _data() {}
    explicit DTPath2D(const DTDoubleArray &input);
    DTPath2D(const DTDoubleArray &input,DTRegion2D crop);
    DTPath2D(const DTIntArray &connections,const DTDoubleArray &points);
    
    bool IsEmpty() const {return _data.IsEmpty();}
    bool NotEmpty() const {return _data.NotEmpty();}
    int NumberOfPoints(void) const; // If loop is closed, does not count the repeated point.
	bool IsClosed(void) const;
	
    int NumberOfLoops(void) const;
    bool StoredAsSegments(void) const {return (_data.IsEmpty() || _data.m()==4);}
    bool StoredAsLoops(void) const {return (_data.IsEmpty() || _data.m()==2);}
    
    DTDoubleArray Data(void) const {return _data;}
    
    DTRegion2D CroppingBox(void) const {return _cropBox;}
    
    void pinfo(void) const;
    void pall(void) const;
    
private:
    bool SetStorage(const DTDoubleArray &input);
    
    DTRegion2D _cropBox;
    DTDoubleArray _data;
};

extern DTRegion2D BoundingBox(const DTPath2D &);

extern bool operator==(const DTPath2D &,const DTPath2D &);
extern bool operator!=(const DTPath2D &,const DTPath2D &);

extern DTPoint2D FirstPoint(const DTPath2D &);
extern DTPoint2D LastPoint(const DTPath2D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPath2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPath2D &thePath);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPath2D &toWrite); // One time value, self documenting.

// A utility class to enable grid sharing.
struct DTPath2D_SaveInfo {
    DTPath2D path;
    std::string name;
};

// Pass in as the third argument to Read and Write to enable the the read/write routines
// to avoid having to read or write identical grids.  This is used when reading DTTrianglarMesh2D and DTTriangularVectorField2D
extern void Read(const DTDataStorage &input,const std::string &name,DTPath2D &toReturn,DTPath2D_SaveInfo &);
extern void Write(DTDataStorage &output,const std::string &name,const DTPath2D &toWrite,DTPath2D_SaveInfo &);

// Utility functions
extern DTPath2D ExtractLoop(const DTPath2D &,int i);
extern DTPath2D ChangeOrientation(const DTPath2D &);
extern DTMutableDoubleArray Points(const DTPath2D &); // Strips any connectivity.  Does not repeat endpoints in a closed loop.
extern DTMutableIntArray SegmentsAsOffsets(const DTPath2D &); // 2xN list, offsets into Points() array.
extern DTMutableIntArray PointOffsets(const DTPath2D &);
extern DTPath2D Combine(const DTList<DTPath2D> &);
extern DTMutableList<DTPath2D> Separate(const DTPath2D &);
extern DTPath2D SegmentForm(const DTPath2D &P); // Free if already stored as segments.
extern DTPath2D LoopForm(const DTPath2D &); // Free if already as loops.
extern DTIntArray ConnectSegmentIndices(const DTIntArray &); // Takes a 2xN loop of int pairs, and ties them up to a 1xM loop structure.
extern DTPath2D operator+(const DTPath2D &,const DTPath2D &); // Combine two paths.
extern double Length(const DTPath2D &);
extern double Area(const DTPath2D &);
extern DTMutableDoubleArray Areas(const DTPath2D &);

// To create simple paths.
extern DTPath2D CreatePath(const DTRegion2D &);
extern DTPath2D Circle(DTPoint2D center,double radius,int howMany); // Counter clockwise

extern DTPath2D SegmentsAsOffsetsAndPoints(const DTIntArray &offsets,const DTDoubleArray &pts);

#endif
