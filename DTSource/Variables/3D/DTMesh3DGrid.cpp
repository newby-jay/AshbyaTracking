// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTMesh3DGrid.h"

#include "DTError.h"
#include "DTIntArray.h"
#include "DTDoubleArray.h"
#include "DTRegion3D.h"
#include "DTUtilities.h"

DTMesh3DGrid::DTMesh3DGrid(ssize_t m,ssize_t n,ssize_t o)
{
    isEmpty = false;
    mv = nv = ov = 0;
    _dx = _dy = _dz = 1.0;
    origin = DTPoint3D(0,0,0);
    _maskDefined = false;

    if (m<0 || n<0 || o<0) {
        DTErrorMessage("DTMeshGrid(m,n)","One of the size is negative.");
        isEmpty = true;
        return;
    }
    if (m*n*o==0)
        m = n = o = 0;
    mv = int(m);
    nv = int(n);
    ov = int(o);
}

DTMesh3DGrid::DTMesh3DGrid(DTPoint3D orig,double dxin,double dyin,double dzin,ssize_t m,ssize_t n,ssize_t o)
{
    isEmpty = false;
    mv = nv = ov = 0;
    _dx = _dy = _dz = 1.0;
    _maskDefined = false;

    if (m<0 || n<0 || o<0) {
        DTErrorMessage("DTMeshGrid(origin,dx,dy,m,n)","One of the size is negative.");
        isEmpty = true;
        return;
    }
    if (dxin<0 || dyin<0 || dzin<0) {
        DTErrorMessage("DTMesh3DGrid(origin,dx,dy,dz,m,n,o)","One of the size is negative.");
        isEmpty = true;
        return;
    }
    if (dxin==0 || dyin==0 || dzin==0) {
        DTErrorMessage("DTMesh3DGrid(origin,dx,dy,dz,m,n,o)","One of the size is zero.");
        isEmpty = true;
        return;
    }
    if (m*n*o==0)
        m = n = o = 0;
    mv = int(m);
    nv = int(n);
    ov = int(o);
    origin = orig;
    _dx = dxin;
    _dy = dyin;
    _dz = dzin;
}

DTMesh3DGrid::DTMesh3DGrid(DTPoint3D orig,double dxin,double dyin,double dzin,const DTMask &theMask)
{
    isEmpty = false;
    mv = nv = ov = 0;
    _dx = _dy = _dz = 1.0;

    if (dxin<0 || dyin<0 || dzin<0) {
        DTErrorMessage("DTMesh3DGrid(origin,dx,dy,dz,mask)","One of the size is negative.");
        isEmpty = true;
        return;
    }
    if (dxin==0 || dyin==0 || dzin==0) {
        DTErrorMessage("DTMesh3DGrid(origin,dx,dy,dz,mask)","One of the size is zero.");
        isEmpty = true;
        return;
    }

    _maskDefined = true;

    mv = theMask.m();
    nv = theMask.n();
    ov = theMask.o();
    origin = orig;
    _dx = dxin;
    _dy = dyin;
    _dz = dzin;
    mask = theMask;
}

DTMesh3DGrid::DTMesh3DGrid(DTPoint3D orig,double dxin,double dyin,double dzin,const DTMask3D &theMask)
{
    isEmpty = false;
    mv = nv = ov = 0;
    _dx = _dy = _dz = 1.0;
    
    if (dxin<0 || dyin<0 || dzin<0) {
        DTErrorMessage("DTMesh3DGrid(origin,dx,dy,dz,mask)","One of the size is negative.");
        isEmpty = true;
        return;
    }
    if (dxin==0 || dyin==0 || dzin==0) {
        DTErrorMessage("DTMesh3DGrid(origin,dx,dy,dz,mask)","One of the size is zero.");
        isEmpty = true;
        return;
    }
    
    _maskDefined = true;
    
    mv = theMask.m();
    nv = theMask.n();
    ov = theMask.o();
    origin = orig;
    _dx = dxin;
    _dy = dyin;
    _dz = dzin;
    _mask = theMask;
}

ssize_t DTMesh3DGrid::NumberOfPointsInMask(void) const
{
    if (_maskDefined==false) return 0;
    if (mask.IsEmpty()) {
        return _mask.NumberOfPoints();
    }
    else {
        return mask.NumberOfPoints();
    }
}

DTMask DTMesh3DGrid::Mask(void) const
{
    if (!_maskDefined) {
        DTErrorMessage("DTMesh3DGrid::Mask","Mask is not defined");
    }
    if (_mask.NotEmpty()) {
        DTErrorMessage("DTMesh3DGrid::Mask","Using a 3D mask");
    }
    return mask;
}

DTMask3D DTMesh3DGrid::Mask3D(void) const
{
    if (!_maskDefined) {
        if (IsEmpty()==false) {
            DTErrorMessage("DTMesh3DGrid::Mask3D","Mask is not defined");
        }
    }
    return _mask;
}

void DTMesh3DGrid::pinfo(void) const
{
    std::cerr << "Size = " << m() << " x " << n() << " x " << o() << ", ";
    DTPoint3D Or = Origin();
    std::cerr << "Origin = (" << Or.x << "," << Or.y << "," << Or.z << "), ";
    if (dx()==dy() && dy()==dz())
        std::cerr << "h = " << dx();
    else {
        std::cerr << "dx = " << dx();
        std::cerr << ", dy = " << dy();
        std::cerr << ", dz = " << dz();
    }
    if (MaskDefined()) {
        std::cerr << ". Mask = " << NumberOfPointsInMask() << " points.";
    }
    std::cerr << std::endl << std::flush;
}

bool operator==(const DTMesh3DGrid &A,const DTMesh3DGrid &B)
{
    if (A.MaskDefined()!=B.MaskDefined())
        return false;
    if (A.m()!=B.m() || A.n()!=B.n() || A.o()!=B.o())
        return false;
    if (A.dx()!=B.dx() || A.dy()!=B.dy() || A.dz()!=B.dz())
        return false;
	// Allow roundoff for the origin.
	DTPoint3D AO = A.Origin();
	DTPoint3D BO = B.Origin();
	if (fabs(AO.x-BO.x)>1e-6*A.dx()) return false;
	if (fabs(AO.y-BO.y)>1e-6*A.dy()) return false;
	if (fabs(AO.z-BO.z)>1e-6*A.dz()) return false;
    if (A.MaskDefined()) {
        return (A.Mask()==B.Mask());
    }
    else {
        return true;
    }
}

bool operator!=(const DTMesh3DGrid &A,const DTMesh3DGrid &B)
{
    return (!(A==B));
}

bool SameExceptForMask(const DTMesh3DGrid &A,const DTMesh3DGrid &B)
{
    if (A.m()!=B.m() || A.n()!=B.n() || A.o()!=B.o())
        return false;
    if (A.dx()!=B.dx() || A.dy()!=B.dy() || A.dz()!=B.dz())
        return false;
	DTPoint3D AO = A.Origin();
	DTPoint3D BO = B.Origin();
	if (fabs(AO.x-BO.x)>1e-10*A.dx() || fabs(AO.y-BO.y)>1e-10*A.dy() || fabs(AO.z-BO.z)>1e-10*A.dz()) return false; // Allow for a small rounding error shift.
    
    return true;
}

DTRegion3D BoundingBox(const DTMesh3DGrid &v)
{
    if (v.m()==0)
        return DTRegion3D();

    DTPoint3D origin = v.Origin();
    
    return DTRegion3D(origin,DTPoint3D(origin.x+v.dx()*(v.m()-1),
                                       origin.y+v.dy()*(v.n()-1),
                                       origin.z+v.dz()*(v.o()-1)));
}

DTMesh3DGrid ChangeSize(const DTMesh3DGrid &Grid,ssize_t m,ssize_t n,ssize_t o)
{
    return DTMesh3DGrid(Grid.Origin(),Grid.dx(),Grid.dy(),Grid.dz(),m,n,o);
}

DTMesh3DGrid GridInsideBox(const DTRegion3D &bbox,double h)
{
    DTPoint3D origin(bbox.xmin,bbox.ymin,bbox.zmin);
    int m = int(round((bbox.xmax-bbox.xmin)/h))+1;
    int n = int(round((bbox.ymax-bbox.ymin)/h))+1;
    int o = int(round((bbox.zmax-bbox.zmin)/h))+1;
    return DTMesh3DGrid(origin,h,h,h,m,n,o);
}

DTMesh3DGrid ApplyMask(const DTMesh3DGrid &grid,const DTMask &mask)
{
    if (grid.m()!=mask.m() || grid.n()!=mask.n() || grid.o()!=mask.o()) {
        DTErrorMessage("ApplyMask(Mesh3DGrid,Mask","Incompatible mask size");
        return DTMesh3DGrid();
    }
    return DTMesh3DGrid(grid.Origin(),grid.dx(),grid.dy(),grid.dz(),mask);
}

DTMesh3DGrid ApplyMask(const DTMesh3DGrid &grid,const DTMask3D &mask)
{
    if (grid.m()!=mask.m() || grid.n()!=mask.n() || grid.o()!=mask.o()) {
        DTErrorMessage("ApplyMask(Mesh3DGrid,Mask","Incompatible mask size");
        return DTMesh3DGrid();
    }
    return DTMesh3DGrid(grid.Origin(),grid.dx(),grid.dy(),grid.dz(),mask);
}

DTMesh3DGrid RemoveMask(const DTMesh3DGrid &Grid)
{
    return DTMesh3DGrid(Grid.Origin(),Grid.dx(),Grid.dy(),Grid.dz(),Grid.m(),Grid.n(),Grid.o());
}

DTMesh3DGrid Crop(const DTMesh3DGrid &Grid,const DTRegion3D &B)
{
	if (Grid.IsEmpty()) return Grid;
	
	double dx = Grid.dx();
	double dy = Grid.dy();
	double dz = Grid.dz();
	ssize_t m = Grid.m();
	ssize_t n = Grid.n();
	ssize_t o = Grid.o();

    int shiftM = 0,shiftN = 0, shiftO = 0;
	DTPoint3D O = Grid.Origin();
	double xz = O.x;
	if (xz<B.xmin) {
        shiftM = int(ceil((B.xmin-O.x)/dx-1e-6));
        xz = O.x + shiftM*dx;
        if (m>0) m-=shiftM;
    }
	double yz = O.y;
	if (yz<B.ymin) {
        shiftN = int(ceil((B.ymin-O.y)/dy-1e-6));
        yz = O.y + shiftN*dy;
        if (n>0) n-=shiftN;
    }
	double zz = O.z;
	if (zz<B.zmin) {
        shiftO = int(ceil((B.zmin-O.z)/dz-1e-6));
        zz = O.z + shiftO*dz;
        if (o>0) o-=shiftO;
    }
	
	if (m==0 && n==0 && o==0) {
        m = int(floor((B.xmax-xz)/dx+1e-10));
	}
    else {
        if (xz+dx*(m-1)>B.xmax)
            m = int(floor((B.xmax-xz)/dx+1e-6))+1; // trim
        if (yz+dy*(n-1)>B.ymax)
            n = int(floor((B.ymax-yz)/dy+1e-6))+1;
        if (zz+dz*(o-1)>B.zmax)
            o = int(floor((B.zmax-zz)/dz+1e-6))+1;
    }
    if (m<0 || n<0 || o<0)
        return DTMesh3DGrid();
    
    return DTMesh3DGrid(DTPoint3D(xz,yz,zz),dx,dy,dz,m,n,o);
}

DTMesh3DGrid KRegion(const DTMesh3DGrid &grid,const DTRange &range)
{
    if (range.start+range.length>grid.o()) {
        DTErrorMessage("KRegion(Mesh3DGrid,Range)","Out of bounds");
        return DTMesh3DGrid();
    }
    
    DTPoint3D origin = grid.Origin();
    origin.z = origin.z + range.start*grid.dz();
    
    return DTMesh3DGrid(origin,grid.dx(),grid.dy(),grid.dz(),grid.m(),grid.n(),range.length);
}

bool Read(const DTDataStorage &input,const std::string &name,DTMesh3DGrid &toReturn)
{
    DTDoubleArray a = input.ReadDoubleArray(name);
    if (a.IsEmpty()) {
        toReturn = DTMesh3DGrid();
        return input.Contains(name);
    }
    if (a.Length()!=6) {
        DTErrorMessage("ReadFromArray(DTMesh3DGrid)","Incorrect size array.");
        toReturn = DTMesh3DGrid();
        return false;
    }

    DTIntArray size;
    std::string sizeName = name+"_size";
    if (input.Contains(sizeName))
        size = input.ReadIntArray(sizeName);
    if (size.Length()>0 && size.Length()!=3) {
        DTErrorMessage("ReadFromArray(DTMesh3DGrid)","Incorrect size array.");
        toReturn = DTMesh3DGrid();
        return false;
    }

    if (size.Length()==0)
        toReturn = DTMesh3DGrid(DTPoint3D(a(0),a(1),a(2)),a(3),a(4),a(5),0,0,0);
    else
        toReturn = DTMesh3DGrid(DTPoint3D(a(0),a(1),a(2)),a(3),a(4),a(5),size(0),size(1),size(2));
    
    if (input.Contains(name+"_mask")) {
        DTMask mask;
        Read(input,name+"_mask",mask);
        toReturn = ApplyMask(toReturn,mask);
    }

    return true;
}

void Write(DTDataStorage &output,const std::string &name,const DTMesh3DGrid &theVar)
{
    // First save [xzero yzero dx dy]
    DTMutableDoubleArray base(6);
    DTPoint3D Orig = theVar.Origin();
    base(0) = Orig.x;
    base(1) = Orig.y;
    base(2) = Orig.z;
    base(3) = theVar.dx();
    base(4) = theVar.dy();
    base(5) = theVar.dz();

    // Save the size if it's not zero.
    if (theVar.m()) {
        DTMutableIntArray size(3);
        size(0) = (int)theVar.m();
        size(1) = (int)theVar.n();
        size(2) = (int)theVar.o();
        output.Save(size,name+"_size");
    }
    
    if (theVar.MaskDefined()) {
        Write(output,name+"_mask",theVar.Mask());
    }
    
    output.Save(base,name);
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh3DGrid &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Mesh Grid");
    output.Flush();
}

void WriteNoSize(DTDataStorage &output,const std::string &name,const DTMesh3DGrid &theVar)
{
    // First save [xzero yzero dx dy]
    DTMutableDoubleArray base(6);
    DTPoint3D Orig = theVar.Origin();
    base(0) = Orig.x;
    base(1) = Orig.y;
    base(2) = Orig.z;
    base(3) = theVar.dx();
    base(4) = theVar.dy();
    base(5) = theVar.dz();
    
    output.Save(base,name);
}
