// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTBitmap2D_Header
#define DTBitmap2D_Header

/*
 A bitmap wrapper.  At this time only Gray scale and RGB bitmaps are supported, with an optional Alpha channel.
 The alpha is premultiplied, which mirrors the way it's stored in DataTank and what the Quartz drawing
 routines expect.
 
 The bit depth is either 8 or 16 bits, stored as a unsigned char array or unsigned short array.
 If you only support 8 bit calculations, add the line
 
 mybitmap = ConvertTo8Bits(mybitmap);
 
 to make sure that the bitmap is 8 bits.  This will do nothing if the bitmap is 8 bits.
 
 NOTE:
 The bitmap is stored with space coordinates and not as scan lines.  Pixel (i,j) corresponds to
 the physical location (x0 + i*dx,y0 + j*dy)
 where the origin (x0,y0) and the step size (dx,dy) can be extracted from the DTMesh2DGrid object.
 
 Because the data is stored in column major, this means that memory-wise, the data is stored
 as lines starting in the lower left corner, going to the right and then up.
 That means that a standard scan-line data set has to be flipped.
 
 */

#include "DTUCharArray.h"
#include "DTUShortIntArray.h"
#include "DTMesh2DGrid.h"

struct DTRegion2D;

class DTBitmap2D {
public:
    DTBitmap2D();
    DTBitmap2D(const DTMesh2DGrid &grid,const DTUCharArray &G);
    DTBitmap2D(const DTMesh2DGrid &grid,const DTUCharArray &G,const DTUCharArray &A);
    DTBitmap2D(const DTMesh2DGrid &grid,const DTUCharArray &R,const DTUCharArray &G,const DTUCharArray &B);
    DTBitmap2D(const DTMesh2DGrid &grid,const DTUCharArray &R,const DTUCharArray &G,const DTUCharArray &B,const DTUCharArray &A);

    DTBitmap2D(const DTMesh2DGrid &grid,const DTUShortIntArray &G);
    DTBitmap2D(const DTMesh2DGrid &grid,const DTUShortIntArray &G,const DTUShortIntArray &A);
    DTBitmap2D(const DTMesh2DGrid &grid,const DTUShortIntArray &R,const DTUShortIntArray &G,const DTUShortIntArray &B);
    DTBitmap2D(const DTMesh2DGrid &grid,const DTUShortIntArray &R,const DTUShortIntArray &G,const DTUShortIntArray &B,const DTUShortIntArray &A);

    bool IsGrayScale(void) const {return (gray16.NotEmpty() || gray.NotEmpty());}
    bool Is16Bits(void) const {return (gray16.NotEmpty() || red16.NotEmpty());}
    bool HasAlpha(void) const {return (alpha.NotEmpty() || alpha16.NotEmpty());}
    bool IsEmpty(void) const {return m()==0;}

    DTMesh2DGrid Grid(void) const {return grid;}
    size_t m(void) const {return grid.m();}
    size_t n(void) const {return grid.n();}

    DTUCharArray Gray(void) const {return gray;}
    DTUCharArray Red(void) const {return red;}
    DTUCharArray Green(void) const {return green;}
    DTUCharArray Blue(void) const {return blue;}
    DTUCharArray Alpha(void) const {return alpha;}

    DTUShortIntArray Gray16(void) const {return gray16;}
    DTUShortIntArray Red16(void) const {return red16;}
    DTUShortIntArray Green16(void) const {return green16;}
    DTUShortIntArray Blue16(void) const {return blue16;}
    DTUShortIntArray Alpha16(void) const {return alpha16;}

    void pinfo(void) const;
private:
    DTMesh2DGrid grid;
    
    DTUCharArray gray;
    DTUCharArray red;
    DTUCharArray green;
    DTUCharArray blue;
    DTUCharArray alpha;
    
    DTUShortIntArray gray16;
    DTUShortIntArray red16;
    DTUShortIntArray green16;
    DTUShortIntArray blue16;
    DTUShortIntArray alpha16;
};

extern DTBitmap2D CreateRGBBitmap(const DTMesh2DGrid &grid,const DTUCharArray &bytes);

extern DTRegion2D BoundingBox(const DTBitmap2D &);
extern DTBitmap2D ConvertTo8Bits(const DTBitmap2D &);
extern DTBitmap2D ConvertToGrayscale(const DTBitmap2D &);

extern void Read(const DTDataStorage &input,const std::string &name,DTBitmap2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTBitmap2D &theBitmap);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTBitmap2D &toWrite); // One time value, self documenting.

#endif
