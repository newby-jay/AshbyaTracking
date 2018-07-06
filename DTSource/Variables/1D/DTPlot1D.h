// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTPlot1D_H
#define DTPlot1D_H

#include "DTDoubleArray.h"
#include "DTDataStorage.h"

// Layout is
// [ 0 x1 .... xN 0 x1 ... xM ...]
// [ N y1 .... yN M y1 ... yM ...]
// This allows multiple disconnected segments to be saved in a single array.

/*
 A standard traversal code will look like:
 
 DTDoubleArray loops = plot.Data();
 ssize_t loc = 0;
 ssize_t len = loops.n();
 ssize_t ptN,loopLength,loopStarts,loopEnds;
 while (loc<len) {
     loopLength = int(loops(1,loc));
     loopStarts = loc+1;
     loopEnds = loc+1+loopLength;
     for (ptN=loopStarts;ptN<loopEnds;ptN++) {
         // x = loops(0,ptN);
         // y = loops(1,ptN);
     }
     loc = loopEnds; // Prepare for the next loop.
 }
 */

struct DTRegion1D;

class DTPlot1D {
public:
    DTPlot1D() : data() {};
    explicit DTPlot1D(const DTDoubleArray &input); // Shares this array.
    DTPlot1D(const DTDoubleArray &x,const DTDoubleArray &y); // A single loop.

    bool IsEmpty(void) const {return data.IsEmpty();}
    DTDoubleArray Data(void) const {return data;}

    ssize_t NumberOfLoops(void) const;
    ssize_t NumberOfPoints(void) const;
    bool UniformlySpaced(void) const; // Single loop and uniform step size.
    
    DTPlot1D ExtractLoop(ssize_t) const;
    bool ContainsX(double) const; // true if an x interval contains this point.
    
    DTMutableDoubleArray XValues(void) const;
    DTMutableDoubleArray YValues(void) const;
    
    void pinfo(void) const;
    
private:
    DTDoubleArray data;
};

extern bool operator==(const DTPlot1D &,const DTPlot1D &);
extern bool operator!=(const DTPlot1D &,const DTPlot1D &);

extern DTRegion1D BoundingRange(const DTPlot1D &v);
extern DTRegion1D ValueRange(const DTPlot1D &v);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTPlot1D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTPlot1D &thePlot);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTPlot1D &toWrite); // One time value, self documenting.

#endif
