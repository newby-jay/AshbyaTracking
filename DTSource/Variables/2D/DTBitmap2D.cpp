// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTBitmap2D.h"
#include "DTError.h"
#include "DTRegion2D.h"
#include "DTDoubleArray.h"

DTBitmap2D::DTBitmap2D()
: grid(), gray(), red(), green(), blue(), alpha(), gray16(), red16(), green16(), blue16(), alpha16()
{
}

DTBitmap2D::DTBitmap2D(const DTMesh2DGrid &Gr,const DTUCharArray &G)
: grid(), gray(), red(), green(), blue(), alpha(), gray16(), red16(), green16(), blue16(), alpha16()
{
    if (G.o()>1) {
        DTErrorMessage("DTBitmap2D(grid,G)","Incorrect array sizes.");
        return;
    }
    gray = G;
    grid = ChangeSize(Gr,G.m(),G.n());
}

DTBitmap2D::DTBitmap2D(const DTMesh2DGrid &Gr,const DTUCharArray &G,const DTUCharArray &A)
: grid(), gray(), red(), green(), blue(), alpha(), gray16(), red16(), green16(), blue16(), alpha16()
{
    if (G.m()!=A.m() || G.n()!=A.n() || G.o()>1 || A.o()>1) {
        DTErrorMessage("DTBitmap2D(grid,G,A)","Incorrect array sizes.");
        return;
    }

    gray = G;
    alpha = A;
    grid = ChangeSize(Gr,G.m(),G.n());
}

DTBitmap2D::DTBitmap2D(const DTMesh2DGrid &Gr,const DTUCharArray &R,const DTUCharArray &G,const DTUCharArray &B)
: grid(), gray(), red(), green(), blue(), alpha(), gray16(), red16(), green16(), blue16(), alpha16()
{
    if (R.m()!=G.m() || R.m()!=B.m() || R.n()!=G.n() || R.n()!=B.n() ||
        R.o()>1 || G.o()>1 || B.o()>1) {
        DTErrorMessage("DTBitmap2D(grid,R,G,B)","Incorrect array sizes.");
        return;
    }

    red = R;
    green = G;
    blue = B;
    grid = ChangeSize(Gr,R.m(),R.n());
}

DTBitmap2D::DTBitmap2D(const DTMesh2DGrid &Gr,const DTUCharArray &R,const DTUCharArray &G,const DTUCharArray &B,const DTUCharArray &A)
: grid(), gray(), red(), green(), blue(), alpha(), gray16(), red16(), green16(), blue16(), alpha16()
{
    if (R.m()!=G.m() || R.m()!=B.m() || R.m()!=A.m() ||
        R.n()!=G.n() || R.n()!=B.n() || R.n()!=A.n() ||
        R.o()>1 || G.o()>1 || B.o()>1 || A.o()>1) {
        DTErrorMessage("DTBitmap2D(grid,R,G,B,A)","Incorrect array sizes.");
        return;
    }

    red = R;
    green = G;
    blue = B;
    alpha = A;
    grid = ChangeSize(Gr,R.m(),R.n());
}

DTBitmap2D::DTBitmap2D(const DTMesh2DGrid &Gr,const DTUShortIntArray &G)
: grid(), gray(), red(), green(), blue(), alpha(), gray16(), red16(), green16(), blue16(), alpha16()
{
    if (G.o()>1) {
        DTErrorMessage("DTBitmap2D(grid,G)","Incorrect array sizes.");
        return;
    }
    gray16 = G;
    grid = ChangeSize(Gr,G.m(),G.n());
}

DTBitmap2D::DTBitmap2D(const DTMesh2DGrid &Gr,const DTUShortIntArray &G,const DTUShortIntArray &A)
: grid(), gray(), red(), green(), blue(), alpha(), gray16(), red16(), green16(), blue16(), alpha16()
{
    if (G.m()!=A.m() || G.n()!=A.n() || G.o()>1 || A.o()>1) {
        DTErrorMessage("DTBitmap2D(grid,G,A)","Incorrect array sizes.");
        return;
    }

    gray16 = G;
    alpha16 = A;
    grid = ChangeSize(Gr,G.m(),G.n());
}

DTBitmap2D::DTBitmap2D(const DTMesh2DGrid &Gr,const DTUShortIntArray &R,const DTUShortIntArray &G,const DTUShortIntArray &B)
: grid(), gray(), red(), green(), blue(), alpha(), gray16(), red16(), green16(), blue16(), alpha16()
{
    if (R.m()!=G.m() || R.m()!=B.m() || R.n()!=G.n() || R.n()!=B.n() ||
        R.o()>1 || G.o()>1 || B.o()>1) {
        DTErrorMessage("DTBitmap2D(grid,R,G,B)","Incorrect array sizes.");
        return;
    }

    red16 = R;
    green16 = G;
    blue16 = B;
    grid = ChangeSize(Gr,R.m(),R.n());
}

DTBitmap2D::DTBitmap2D(const DTMesh2DGrid &Gr,const DTUShortIntArray &R,const DTUShortIntArray &G,const DTUShortIntArray &B,const DTUShortIntArray &A)
: grid(), gray(), red(), green(), blue(), alpha(), gray16(), red16(), green16(), blue16(), alpha16()
{
    if (R.m()!=G.m() || R.m()!=B.m() || R.m()!=A.m() ||
        R.n()!=G.n() || R.n()!=B.n() || R.n()!=A.n() ||
        R.o()>1 || G.o()>1 || B.o()>1 || A.o()>1) {
        DTErrorMessage("DTBitmap2D(grid,R,G,B,A)","Incorrect array sizes.");
        return;
    }

    red16 = R;
    green16 = G;
    blue16 = B;
    alpha16 = A;
    grid = ChangeSize(Gr,R.m(),R.n());
}

void DTBitmap2D::pinfo(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty" << std::endl << std::flush;
    }
    else {
        if (IsGrayScale()) {
            std::cerr << "Grayscale - ";
        }
        else {
            std::cerr << "RGB - ";
        }
        if (Is16Bits()) {
            std::cerr << "16 bits - ";
        }

        if (grid.dx()==grid.dy()) {
            std::cerr << m() << "x" << n() << ", origin = (" << grid.Origin().x << "," << grid.Origin().y << "), h = " << grid.dx();
        }
        else {
            std::cerr << m() << "x" << n() << ", origin = (" << grid.Origin().x << "," << grid.Origin().y << "), dx = " << grid.dx() << ", dy = " << grid.dy();
        }
        
        if (grid.MaskDefined()) {
            std::cerr << ". Mask = " << grid.Mask().NumberOfPoints() << " points.";
        }
        std::cerr << std::endl << std::flush;
    }
}

DTBitmap2D CreateRGBBitmap(const DTMesh2DGrid &grid,const DTUCharArray &bytes)
{
    if (bytes.IsEmpty())
        return DTBitmap2D();
    int m = grid.m();
    int n = grid.n();
    if (m*n==0) {
        return DTBitmap2D();
    }

    int perPixel = bytes.Length()/(m*n);
    if (perPixel*m*n!=bytes.Length() || (perPixel!=3 && perPixel!=4)) {
        DTErrorMessage("CreateRGBBitmap(grid,bytes)","Array size is invalid");
        return DTBitmap2D();
    }
    
    ssize_t i,len = m*n;
    if (perPixel==3) {
        DTMutableUCharArray r(m,n), g(m,n), b(m,n);
        for (i=0;i<len;i++) {
            r(i) = bytes(3*i);
            g(i) = bytes(3*i+1);
            b(i) = bytes(3*i+2);
        }
        return DTBitmap2D(grid,r,g,b);
    }
    else {
        DTMutableUCharArray r(m,n), g(m,n), b(m,n), a(m,n);
        for (i=0;i<len;i++) {
            r(i) = bytes(4*i);
            g(i) = bytes(4*i+1);
            b(i) = bytes(4*i+2);
            a(i) = bytes(4*i+3);
        }
        return DTBitmap2D(grid,r,g,b,a);
    }
}

DTRegion2D BoundingBox(const DTBitmap2D &v)
{
    return BoundingBox(v.Grid());
}

DTBitmap2D ConvertTo8Bits(const DTBitmap2D &B)
{
    if (!B.Is16Bits())
        return B;
    if (B.IsEmpty())
        return B;
    
    DTUShortIntArray Gray = B.Gray16();
    const int m = int(B.m());
    const int n = int(B.n());
    const int mn = m*n;
    int ij;
    unsigned char *to;
    const unsigned short int *from;

    DTBitmap2D toReturn;
    
    if (Gray.NotEmpty()) {
        DTMutableUCharArray Gray8(m,n);
        
        from = Gray.Pointer();
        to = Gray8.Pointer();
        for (ij=0;ij<mn;ij++)
            to[ij] = from[ij]/256;
        
        toReturn = DTBitmap2D(B.Grid(),Gray8);
    }
    else {
        DTMutableUCharArray Red8(m,n);
        DTMutableUCharArray Green8(m,n);
        DTMutableUCharArray Blue8(m,n);

        from = B.Red16().Pointer();
        to = Red8.Pointer();
        for (ij=0;ij<mn;ij++)
            to[ij] = from[ij]/256;

        from = B.Green16().Pointer();
        to = Green8.Pointer();
        for (ij=0;ij<mn;ij++)
            to[ij] = from[ij]/256;

        from = B.Blue16().Pointer();
        to = Blue8.Pointer();
        for (ij=0;ij<mn;ij++)
            to[ij] = from[ij]/256;

        toReturn = DTBitmap2D(B.Grid(),Red8,Green8,Blue8);
    }

    return toReturn;
}

DTBitmap2D ConvertToGrayscale(const DTBitmap2D &B)
{
    // Weight r = 0.299, g = 0.587, b = 0.114  - Exactly the same as DataTank does.
    if (B.IsEmpty()) return B;
    if (B.IsGrayScale()) return B;
    
    DTBitmap2D toReturn;
    DTMesh2DGrid grid = B.Grid();
    if (B.Is16Bits()) {
        DTUShortIntArray r = B.Red16();
        DTUShortIntArray g = B.Green16();
        DTUShortIntArray b = B.Blue16();
        const unsigned short *rD = r.Pointer();
        const unsigned short *gD = g.Pointer();
        const unsigned short *bD = b.Pointer();
        DTMutableUShortIntArray gray(grid.m(),grid.n());
        unsigned short *grayD = gray.Pointer();
        int ij,mn = int(r.Length());
        for (ij=0;ij<mn;ij++) {
            grayD[ij] = (unsigned short) round(0.299*rD[ij] + 0.587*gD[ij] + 0.114*bD[ij]);
        }
        if (B.HasAlpha())
            toReturn = DTBitmap2D(grid,gray,B.Alpha16());
        else
            toReturn = DTBitmap2D(grid,gray);
    }
    else {
        DTUCharArray r = B.Red();
        DTUCharArray g = B.Green();
        DTUCharArray b = B.Blue();
        const unsigned char *rD = r.Pointer();
        const unsigned char *gD = g.Pointer();
        const unsigned char *bD = b.Pointer();
        DTMutableUCharArray gray(grid.m(),grid.n());
        unsigned char *grayD = gray.Pointer();
        int ij,mn = int(r.Length());
        for (ij=0;ij<mn;ij++) {
            grayD[ij] = (unsigned char) round(0.299*rD[ij] + 0.587*gD[ij] + 0.114*bD[ij]);
        }
        if (B.HasAlpha())
            toReturn = DTBitmap2D(grid,gray,B.Alpha());
        else
            toReturn = DTBitmap2D(grid,gray);
    }
    
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTBitmap2D &toReturn)
{
    // Location
    DTMesh2DGrid grid;
    Read(input,name,grid);

    DTUCharArray alpha,red,green,blue,gray;
    DTUShortIntArray alpha16,red16,green16,blue16,gray16;

    if (input.Contains(name+"_Gray")) Read(input,name+"_Gray",gray);
    if (input.Contains(name+"_Alpha")) Read(input,name+"_Alpha",alpha);
    if (input.Contains(name+"_Red")) Read(input,name+"_Red",red);
    if (input.Contains(name+"_Green")) Read(input,name+"_Green",green);
    if (input.Contains(name+"_Blue")) Read(input,name+"_Blue",blue);

    if (input.Contains(name+"_Gray16")) Read(input,name+"_Gray16",gray16);
    if (input.Contains(name+"_Alpha16")) Read(input,name+"_Alpha16",alpha16);
    if (input.Contains(name+"_Red16")) Read(input,name+"_Red16",red16);
    if (input.Contains(name+"_Green16")) Read(input,name+"_Green16",green16);
    if (input.Contains(name+"_Blue16")) Read(input,name+"_Blue16",blue16);

    if (gray16.IsEmpty() && red16.IsEmpty()) {
        if (red.NotEmpty() && green.NotEmpty() && blue.NotEmpty()) {
            if (alpha.NotEmpty())
                toReturn = DTBitmap2D(grid,red,green,blue,alpha);
            else
                toReturn = DTBitmap2D(grid,red,green,blue);
        }
        else {
            if (alpha.NotEmpty())
                toReturn = DTBitmap2D(grid,gray,alpha);
            else
                toReturn = DTBitmap2D(grid,gray);
        }
    }
    else {
        if (red16.NotEmpty() && green16.NotEmpty() && blue16.NotEmpty()) {
            if (alpha16.NotEmpty())
                toReturn = DTBitmap2D(grid,red16,green16,blue16,alpha16);
            else
                toReturn = DTBitmap2D(grid,red16,green16,blue16);
        }
        else {
            if (alpha16.NotEmpty())
                toReturn = DTBitmap2D(grid,gray16,alpha16);
            else
                toReturn = DTBitmap2D(grid,gray16);
        }
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTBitmap2D &theBitmap)
{
    if (theBitmap.IsEmpty()) {
        Write(output,name,DTDoubleArray());
        return;
    }
    
    if (theBitmap.Gray().NotEmpty()) Write(output,name+"_Gray",theBitmap.Gray());
    if (theBitmap.Red().NotEmpty()) Write(output,name+"_Red",theBitmap.Red());
    if (theBitmap.Green().NotEmpty()) Write(output,name+"_Green",theBitmap.Green());
    if (theBitmap.Blue().NotEmpty()) Write(output,name+"_Blue",theBitmap.Blue());
    if (theBitmap.Alpha().NotEmpty()) Write(output,name+"_Alpha",theBitmap.Alpha());

    if (theBitmap.Gray16().NotEmpty()) Write(output,name+"_Gray16",theBitmap.Gray16());
    if (theBitmap.Red16().NotEmpty()) Write(output,name+"_Red16",theBitmap.Red16());
    if (theBitmap.Green16().NotEmpty()) Write(output,name+"_Green16",theBitmap.Green16());
    if (theBitmap.Blue16().NotEmpty()) Write(output,name+"_Blue16",theBitmap.Blue16());
    if (theBitmap.Alpha16().NotEmpty()) Write(output,name+"_Alpha16",theBitmap.Alpha16());

    WriteNoSize(output,name,theBitmap.Grid());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTBitmap2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Bitmap");
    output.Flush();
}

