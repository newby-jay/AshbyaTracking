// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTMesh3D.h"

#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion3D.h"
#include "DTArrayConversion.h"
#include "DTFloatArrayOperators.h"
#include "DTDoubleArrayOperators.h"
#include "DTUtilities.h"

#include <cstring>
#include <algorithm>

DTMesh3D::DTMesh3D(const DTMesh3DGrid &gr,const DTFloatArray &input)
{
    if (input.IsEmpty()) return;

    _grid = gr;
    if (gr.m()!=input.m() || gr.n()!=input.n() || gr.o()!=input.o()) {
        _grid = ChangeSize(gr,int(input.m()),int(input.n()),int(input.o()));
    }
    _floatData = input;
}

DTMesh3D::DTMesh3D(const DTMesh3DGrid &gr,const DTDoubleArray &input)
{
    if (input.IsEmpty()) return;

    _grid = gr;
    if (gr.m()!=input.m() || gr.n()!=input.n() || gr.o()!=input.o()) {
        _grid = ChangeSize(gr,int(input.m()),int(input.n()),int(input.o()));
    }
    _doubleData = input;
}

const DTDoubleArray &DTMesh3D::DoubleData(void) const
{
    if (_floatData.NotEmpty()) {
        DTErrorMessage("DTMesh3D::DoubleData","Array saved as float.");
    }
    return _doubleData;
}

const DTFloatArray &DTMesh3D::FloatData(void) const
{
    if (_doubleData.NotEmpty()) {
        DTErrorMessage("DTMesh3D::FloatData","Array saved as double.");
    }
    return _floatData;
}

void DTMesh3D::pinfo(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty.\n";
        return;
    }
    
    std::cerr << "Size = " << m() << " x " << n() << " x " << o() << ", ";
    DTPoint3D Or = _grid.Origin();
    std::cerr << "Origin = (" << Or.x << "," << Or.y << "," << Or.z << "), ";
    if (_grid.dx()==_grid.dy() && _grid.dy()==_grid.dz())
        std::cerr << "h = " << _grid.dx();
    else {
        std::cerr << "dx = " << _grid.dx();
        std::cerr << ", dy = " << _grid.dy();
        std::cerr << ", dz = " << _grid.dz();
    }

    if (FloatPrecision()) {
        std::cerr << ". Float";
    }
    else {
        std::cerr << ". Double";
    }
    
    if (_grid.MaskDefined()) {
        std::cerr << ". Mask = " << _grid.NumberOfPointsInMask() << " points.";
    }
    std::cerr << std::endl;
    std::cerr << std::flush;
}

DTMesh3D ApplyMask(const DTMesh3D &mesh,const DTMask &mask)
{
	if (mesh.m()==mask.m() && mesh.n()==mask.n() && mesh.o()==mask.o()) {
		if (mesh.FloatPrecision()) {
			return DTMesh3D(ApplyMask(mesh.Grid(),mask),mesh.FloatData());
		}
		else {
			return DTMesh3D(ApplyMask(mesh.Grid(),mask),mesh.DoubleData());
		}
	}
	else {
		DTErrorMessage("ApplyMask(Mesh3D,Mask)","Incompatible mask size.");
		return mesh;
	}
}

DTMesh3D RemoveMask(const DTMesh3D &mesh)
{
    if (mesh.FloatPrecision()) {
        return DTMesh3D(RemoveMask(mesh.Grid()),mesh.FloatData());
    }
    else {
        return DTMesh3D(RemoveMask(mesh.Grid()),mesh.DoubleData());
    }
}

DTMesh3D ConvertToFloat(DTMesh3D m)
{
    if (m.FloatPrecision())
        return m;
    else
        return DTMesh3D(m.Grid(),ConvertToFloat(m.DoubleData()));
}

DTMesh3D ConvertToDouble(DTMesh3D m)
{
    if (m.DoublePrecision())
        return m;
    else
        return DTMesh3D(m.Grid(),ConvertToDouble(m.FloatData()));
}

bool CompatibleMeshes(const DTMesh3D &A,const DTMesh3D &B)
{
    if (A.FloatPrecision()!=B.FloatPrecision())
        return false;
	if (A.Grid()!=B.Grid())
		return false;
    return true;
}

bool operator==(const DTMesh3D &A,const DTMesh3D &B)
{
    return (A.Grid()==B.Grid() && A.DoubleData()==B.DoubleData() && A.FloatData()==B.FloatData());
}

bool operator!=(const DTMesh3D &A,const DTMesh3D &B)
{
    return (A.Grid()!=B.Grid() || A.DoubleData()!=B.DoubleData() || A.FloatData()!=B.FloatData());
}

DTMesh3D Crop(const DTMesh3D &mesh,const DTRegion3D &crop)
{
	if (crop.Contains(BoundingBox(mesh))) return mesh;

	DTMesh3DGrid Grid = mesh.Grid();
	double dx = Grid.dx();
	double dy = Grid.dy();
	double dz = Grid.dz();
	ssize_t m = Grid.m();
	ssize_t n = Grid.n();
	ssize_t o = Grid.o();
	
    int shiftM = 0,shiftN = 0, shiftO = 0;
	DTPoint3D O = Grid.Origin();
	double xz = O.x;
	if (xz<crop.xmin) {
        shiftM = int(ceil((crop.xmin-O.x)/dx-1e-6));
        xz = O.x + shiftM*dx;
        if (m>0) m-=shiftM;
    }
	double yz = O.y;
	if (yz<crop.ymin) {
        shiftN = int(ceil((crop.ymin-O.y)/dy-1e-6));
        yz = O.y + shiftN*dy;
        if (n>0) n-=shiftN;
    }
	double zz = O.z;
	if (zz<crop.zmin) {
        shiftO = int(ceil((crop.zmin-O.z)/dz-1e-6));
        zz = O.z + shiftO*dz;
        if (o>0) o-=shiftO;
    }
	
	if (m==0 && n==0 && o==0) {
        m = int(floor((crop.xmax-xz)/dx+1e-10));
	}
    else {
        if (xz+dx*(m-1)>crop.xmax)
            m = int(floor((crop.xmax-xz)/dx+1e-6))+1; // trim
        if (yz+dy*(n-1)>crop.ymax)
            n = int(floor((crop.ymax-yz)/dy+1e-6))+1;
        if (zz+dz*(o-1)>crop.zmax)
            o = int(floor((crop.zmax-zz)/dz+1e-6))+1;
    }
    if (m<0 || n<0 || o<0)
        return DTMesh3D();
    
    DTMesh3DGrid newGrid(DTPoint3D(xz,yz,zz),dx,dy,dz,m,n,o);
	// Crop the array
	DTMesh3D toReturn;
	if (mesh.DoublePrecision())
		toReturn = DTMesh3D(newGrid,SubArray(mesh.DoubleData(),shiftM,m, shiftN, n, shiftO, o));
	else
		toReturn = DTMesh3D(newGrid,Region(mesh.FloatData(),DTRange(shiftM,m), DTRange(shiftN, n), DTRange(shiftO, o)));
	return toReturn;
}

DTRegion3D BoundingBox(const DTMesh3D &v)
{
    return BoundingBox(v.Grid());
}

DTRegion1D ValueRange(const DTMesh3D &v)
{
    if (v.DoublePrecision())
        return ValueRange(v.DoubleData());
    else
        return ValueRange(v.FloatData());
}

double Minimum(const DTMesh3D &v)
{
    if (v.DoublePrecision())
        return Minimum(v.DoubleData());
    else
        return Minimum(v.FloatData());
}

double Maximum(const DTMesh3D &v)
{
    if (v.DoublePrecision())
        return Maximum(v.DoubleData());
    else
        return Maximum(v.FloatData());
}

DTMesh3D operator-(const DTMesh3D &A,const DTMesh3D &B)
{
	if (A.Grid()!=B.Grid()) {
		DTErrorMessage("DTMesh3D-DTMesh3D","The grids have different dimensions");
		return DTMesh3D();
	}
	if (A.DoublePrecision()!=B.DoublePrecision()) {
		DTErrorMessage("DTMesh3D-DTMesh3D","Both meshes need to be double or float, use ConvertToFloat or ConvertToDouble first");
		return DTMesh3D();
	}
	
	if (A.DoublePrecision()) {
		return DTMesh3D(A.Grid(),A.DoubleData()-B.DoubleData());
	}
	else {
		return DTMesh3D(A.Grid(),A.FloatData()-B.FloatData());
	}
}

void Read(const DTDataStorage &input,const std::string &name,DTMesh3D &toReturn)
{
    DTDoubleArray dData;
    DTFloatArray fData;
    
    if (input.SavedAsDouble(name)) {
        Read(input,name,dData);
    }
    else {
        Read(input,name,fData);
    }
    
    DTMesh3DGrid grid;
    std::string locName = name+"_loc";
    if (input.Contains(locName)) {
        Read(input,locName,grid);
        if (dData.NotEmpty())
            grid = ChangeSize(grid,dData.m(),dData.n(),dData.o());
        else
            grid = ChangeSize(grid,fData.m(),fData.n(),fData.o());
    }
    else {
        if (dData.NotEmpty())
            grid = DTMesh3DGrid(dData.m(),dData.n(),dData.o());
        else
            grid = DTMesh3DGrid(fData.m(),fData.n(),fData.o());
    }
    
    std::string domName = name+"_dom";
    if (input.Contains(domName)) {
        DTMask mask;
        Read(input,domName,mask);
        grid = ApplyMask(grid,mask);
        mask = grid.Mask(); // In case it wasn't valid.
        
        ssize_t howManyPoints = mask.NumberOfPoints();
        if (fData.Length()==howManyPoints || dData.Length()==howManyPoints) {
            // Saved as a flattened array.  Only the points in the domain are saved.
            DTIntArray offsets = mask.Offsets();
            int i;
            if (fData.NotEmpty()) {
                DTMutableFloatArray arr(mask.m(),mask.n(),mask.o());
                arr = 0.0;
                for (i=0;i<howManyPoints;i++) {
                    arr(offsets(i)) = fData(i);
                }
                fData = arr;
            }
            else if (dData.NotEmpty()) {
                DTMutableDoubleArray arr(mask.m(),mask.n(),mask.o());
                arr = 0.0;
                for (i=0;i<howManyPoints;i++) {
                    arr(offsets(i)) = dData(i);
                }
                dData = arr;
            }
        }
    }

    if (fData.NotEmpty())
        toReturn = DTMesh3D(grid,fData);
    else
        toReturn = DTMesh3D(grid,dData);
}

void Write(DTDataStorage &output,const std::string &name,const DTMesh3D &theMesh)
{
    Write(output,name+"_bbox3D",BoundingBox(theMesh));
    
    if (!theMesh.Grid().IsStandard()) {
        WriteNoSize(output,name+"_loc",theMesh.Grid());
    }
    
    if (theMesh.Grid().MaskDefined()) {
        if (theMesh.Grid().Mask3DDefined())
            Write(output,name+"_dom",theMesh.Grid().Mask3D());
        else
            Write(output,name+"_dom",theMesh.Grid().Mask());
    }
    
    if (theMesh.DoublePrecision())
        Write(output,name,theMesh.DoubleData());
    else
        Write(output,name,theMesh.FloatData());
}

void WriteFloat(DTDataStorage &output,const std::string &name,const DTMesh3D &theMesh)
{
    Write(output,name+"_bbox3D",BoundingBox(theMesh));

    if (!theMesh.Grid().IsStandard()) {
        WriteNoSize(output,name+"_loc",theMesh.Grid());
    }

    if (theMesh.Grid().MaskDefined()) {
        if (theMesh.Grid().Mask3DDefined())
            Write(output,name+"_dom",theMesh.Grid().Mask3D());
        else
            Write(output,name+"_dom",theMesh.Grid().Mask());
    }

    if (theMesh.DoublePrecision())
        Write(output,name,ConvertToFloat(theMesh.DoubleData()));
    else
        Write(output,name,theMesh.FloatData());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Mesh");
    output.Flush();
}

#pragma mark Mutable verison

DTMutableMesh3D::DTMutableMesh3D(const DTMesh3DGrid &gr)
{
    _grid = gr;
    _mutableDoubleData = DTMutableDoubleArray(_grid.m(),_grid.n(),_grid.o());
    _doubleData = _mutableDoubleData;
}

DTMutableMesh3D::DTMutableMesh3D(const DTMesh3DGrid &gr,const DTMutableDoubleArray &input)
: DTMesh3D(gr,input)
{
    if (gr==_grid) {
        _mutableDoubleData = input;
    }
}

DTMutableMesh3D::DTMutableMesh3D(const DTMesh3DGrid &gr,const DTMutableFloatArray &input)
: DTMesh3D(gr,input)
{
    if (gr==_grid) {
        _mutableFloatData = input;
    }
}

DTMutableDoubleArray DTMutableMesh3D::DoubleData(void) const
{
    if (_mutableDoubleData.IsEmpty() && _mutableFloatData.NotEmpty()) {
        // Need to convert the array
        DTErrorMessage("MutableMesh3D::DoubleData()","Saved as float");
    }
    return _mutableDoubleData;
}

DTMutableFloatArray DTMutableMesh3D::FloatData(void) const
{
    if (_mutableDoubleData.NotEmpty() && _mutableFloatData.IsEmpty()) {
        // Need to convert the array
        DTErrorMessage("MutableMesh3D::FloatData()","Saved as double");
    }
    return _mutableFloatData;
}

void DTMutableMesh3D::operator-=(const DTMesh3D &A)
{
    if (CompatibleMeshes(*this,A)==false) {
        DTErrorMessage("DTMutableMesh3D -= DTMesh3D","Incompatible meshes");
        return;
    }
    if (FloatPrecision())
        _mutableFloatData -= A.FloatData();
    else
        _mutableDoubleData -= A.DoubleData();
}

void DTMutableMesh3D::operator+=(const DTMesh3D &A)
{
    if (CompatibleMeshes(*this,A)==false) {
        DTErrorMessage("DTMutableMesh3D += DTMesh3D","Incompatible meshes");
        return;
    }
    if (FloatPrecision())
        _mutableFloatData += A.FloatData();
    else
        _mutableDoubleData += A.DoubleData();
}

void DTMutableMesh3D::operator*=(const DTMesh3D &A)
{
    if (CompatibleMeshes(*this,A)==false) {
        DTErrorMessage("MutableMesh3D *= Mesh3D","Incompatible meshes");
        return;
    }
    if (FloatPrecision())
        _mutableFloatData *= A.FloatData();
    else
        _mutableDoubleData *= A.DoubleData();
}

void DTMutableMesh3D::operator*=(const DTDoubleArray &A)
{
    if (FloatPrecision()) {
        DTErrorMessage("DTMutableMesh3D *= DTDoubleArray","mesh is float");
        return;
    }
    else {
        _mutableDoubleData *= A;
    }
}

void DTMutableMesh3D::operator/=(const DTMesh3D &A)
{
    if (CompatibleMeshes(*this,A)==false) {
        DTErrorMessage("DTMutableMesh3D /= DTMesh3D","Incompatible meshes");
        return;
    }
    if (FloatPrecision())
        _mutableFloatData /= A.FloatData();
    else
        _mutableDoubleData /= A.DoubleData();
}

void DTMutableMesh3D::operator*=(double a)
{
    if (_mutableDoubleData.NotEmpty())
        _mutableDoubleData *= a;
    if (_mutableFloatData.NotEmpty())
        _mutableFloatData *= float(a);
}

void DTMutableMesh3D::operator=(double v)
{
    _mutableDoubleData = v;
    _mutableFloatData = float(v);
}

void DTMutableMesh3D::SetMaskedOutValues(double v)
{
	if (_grid.MaskDefined()==false) return;
	DTMask theMask = _grid.Mask();
	DTIntArray intervals = theMask.Intervals();
	ssize_t i,interval,howMany = intervals.n();
	ssize_t startAt = 0,endAt;
    if (DoublePrecision()) {
        double *D = _mutableDoubleData.Pointer();
        for (interval=0;interval<howMany;interval++) {
            endAt = intervals(0,interval);
            for (i=startAt;i<endAt;i++)
                D[i] = v;
            startAt = intervals(1,interval)+1;
        }
        endAt = _mutableDoubleData.Length();
        for (i=startAt;i<endAt;i++)
            D[i] = v;
    }
    else {
        float fv = float(v);
        float *D = _mutableFloatData.Pointer();
        for (interval=0;interval<howMany;interval++) {
            endAt = intervals(0,interval);
            for (i=startAt;i<endAt;i++)
                D[i] = fv;
            startAt = intervals(1,interval)+1;
        }
        endAt = _mutableDoubleData.Length();
        for (i=startAt;i<endAt;i++)
            D[i] = fv;
    }
}

DTMutableMesh3D ConvertToFloat(const DTMutableMesh3D &m)
{
    if (m.FloatPrecision())
        return m;
    else
        return DTMutableMesh3D(m.Grid(),ConvertToFloat(m.DoubleData()));
}

DTMutableMesh3D ConvertToDouble(const DTMutableMesh3D &m)
{
    if (m.DoublePrecision())
        return m;
    else
        return DTMutableMesh3D(m.Grid(),ConvertToDouble(m.FloatData()));
}

DTMutableMesh3D Copy(const DTMesh3D &M)
{
    if (M.DoublePrecision()) {
        return DTMutableMesh3D(M.Grid(),M.DoubleData().Copy());
    }
    else {
        return DTMutableMesh3D(M.Grid(),M.FloatData().Copy());
    }
}

DTMutableMesh3D ApplyMask(const DTMutableMesh3D &mesh,const DTMask &mask)
{
	if (mesh.m()==mask.m() && mesh.n()==mask.n() && mesh.o()==mask.o()) {
		if (mesh.FloatPrecision()) {
			return DTMutableMesh3D(ApplyMask(mesh.Grid(),mask),mesh.FloatData());
		}
		else {
			return DTMutableMesh3D(ApplyMask(mesh.Grid(),mask),mesh.DoubleData());
		}
	}
	else {
		DTErrorMessage("ApplyMask(DTMesh3D,DTMask)","Incompatible mask size.");
		return mesh;
	}
}

DTMutableMesh3D RemoveMask(const DTMutableMesh3D &mesh)
{
    if (mesh.FloatPrecision()) {
        return DTMutableMesh3D(RemoveMask(mesh.Grid()),mesh.FloatData());
    }
    else {
        return DTMutableMesh3D(RemoveMask(mesh.Grid()),mesh.DoubleData());
    }
}

#pragma mark DTMesh3DBanded

DTMesh3DBanded::DTMesh3DBanded(const DTMesh3DGrid &gr,const DTFloatArray &input)
{
    if (input.IsEmpty()) return;

    if (gr.MaskDefined()==false) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Mask needs to be defined for the grid.");
        return;
    }

    if (input.Length()!=input.m()) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Array needs to be a list of numbers.");
        return;
    }
    if (input.Length()!=gr.NumberOfPointsInMask()) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Invalid number of points in array.  Needs to equal mask.NumberOfPoints().");
        return;
    }

    _grid = gr;
    _floatData = input;
}

DTMesh3DBanded::DTMesh3DBanded(const DTMesh3DGrid &gr,const DTDoubleArray &input)
{
    if (input.IsEmpty()) return;
    
    if (gr.MaskDefined()==false) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Mask needs to be defined for the grid.");
        return;
    }
    
    if (input.Length()!=input.m()) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Array needs to be a list of numbers.");
        return;
    }
    if (input.Length()!=gr.NumberOfPointsInMask()) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Invalid number of points in array.  Needs to equal mask.NumberOfPoints().");
        return;
    }
    
    _grid = gr;
    _doubleData = input;
}

void DTMesh3DBanded::pinfo(void) const
{
    if (IsEmpty()) {
        std::cerr << "Empty.\n";
        return;
    }

    if (_grid.dx()==_grid.dy() && _grid.dy()==_grid.dz()) {
        std::cerr << m() << " x " << n() << " x " << o() << " banded mesh, origin = (" << _grid.Origin().x << "," << _grid.Origin().y << "," << _grid.Origin().z << "), h = " << _grid.dx();
    }
    else {
        std::cerr << m() << " x " << n() << " x " << o() << " banded mesh, origin = (" << _grid.Origin().x << "," << _grid.Origin().y << "," << _grid.Origin().z << "), dx = " << _grid.dx() << ", dy = " << _grid.dy() << ", dz = " << _grid.dz();
    }

    std::cerr << ". " << _grid.NumberOfPointsInMask() << " points.\n";
    std::cerr << std::flush;
}

DTMutableMesh3DBanded DTMesh3DBanded::Copy(void) const
{
    if (FloatPrecision()) {
        return DTMutableMesh3DBanded(Grid(),_floatData.Copy());
    }
    else {
        return DTMutableMesh3DBanded(Grid(),_doubleData.Copy());
    }
}

const DTFloatArray &DTMesh3DBanded::FloatData(void) const
{
    return _floatData;
}

const DTFloatArray &DTMutableMesh3DBanded::FloatData(void) const
{
    return _floatData;
}

const DTDoubleArray &DTMesh3DBanded::DoubleData(void) const
{
    return _doubleData;
}

const DTDoubleArray &DTMutableMesh3DBanded::DoubleData(void) const
{
    return _doubleData;
}

#pragma mark DTMutableMesh3DBanded

DTMutableMesh3DBanded::DTMutableMesh3DBanded(const DTMesh3DGrid &gr)
{
    if (gr.MaskDefined()==false) {
        DTErrorMessage("DTMesh3DBanded(grid)","Mask needs to be defined for the grid.");
        return;
    }

    _grid = gr;
    _mutableFloatData = DTMutableFloatArray(gr.NumberOfPointsInMask());
    _floatData = _mutableFloatData;
}

DTMutableMesh3DBanded::DTMutableMesh3DBanded(const DTMesh3DGrid &gr,const DTMutableFloatArray &input)
{
    if (gr.MaskDefined()==false) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Mask needs to be defined for the grid.");
        return;
    }

    if (input.Length()!=input.m()) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Array needs to be a list of numbers.");
        return;
    }
    if (input.Length()!=gr.NumberOfPointsInMask()) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Invalid number of points in array.  Needs to equal mask.NumberOfPoints().");
        return;
    }

    _grid = gr;
    _floatData = input;
    _mutableFloatData = input;
}

DTMutableMesh3DBanded::DTMutableMesh3DBanded(const DTMesh3DGrid &gr,const DTMutableDoubleArray &input)
{
    if (input.IsEmpty()) return;

    if (gr.MaskDefined()==false) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Mask needs to be defined for the grid.");
        return;
    }

    if (input.Length()!=input.m()) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Array needs to be a list of numbers.");
        return;
    }
    if (input.Length()!=gr.NumberOfPointsInMask()) {
        DTErrorMessage("DTMesh3DBanded(grid,values)","Invalid number of points in array.  Needs to equal mask.NumberOfPoints().");
        return;
    }

    _grid = gr;
    _doubleData = input;
    _mutableDoubleData = input;
}

DTMutableFloatArray DTMutableMesh3DBanded::FloatData(void)
{
    return _mutableFloatData;
}

DTMutableDoubleArray DTMutableMesh3DBanded::DoubleData(void)
{
    return _mutableDoubleData;
}

void DTMutableMesh3DBanded::operator=(double v)
{
    _mutableFloatData = float(v);
}

void DTMutableMesh3DBanded::operator*=(double v)
{
    _mutableFloatData *= float(v);
}

void DTMutableMesh3DBanded::operator-=(const DTMesh3DBanded &M)
{
    if (M.Grid()!=Grid()) {
        DTErrorMessage("DTMutableMesh3DBanded-=DTMesh3DBanded","Incompatiable grids");
    }
    else {
        _mutableFloatData -= M.FloatData();
    }
}

void DTMutableMesh3DBanded::operator+=(const DTMesh3DBanded &M)
{
    if (M.Grid()!=Grid()) {
        DTErrorMessage("DTMutableMesh3DBanded+=DTMesh3DBanded","Incompatiable grids");
    }
    else {
        _mutableFloatData += M.FloatData();
    }
}

void DTMutableMesh3DBanded::operator*=(const DTMesh3DBanded &M)
{
    if (M.Grid()!=Grid()) {
        DTErrorMessage("DTMutableMesh3DBanded*=DTMesh3DBanded","Incompatiable grids");
    }
    else {
        _mutableFloatData *= M.FloatData();
    }
}

void operator-=(DTMutableMesh3D &Full,const DTMesh3DBanded &addThis)
{
    if (Full.Grid()!=addThis.Grid()) {
        DTErrorMessage("DTMutableMesh3D-=DTMesh3DBand","Incompatible grids");
        return;
    }

    DTMesh3DGrid grid = Full.Grid();

    if (Full.FloatPrecision() && addThis.FloatPrecision()) {
        // Just change this portion of the mesh
        DTIntArray intervals = grid.Mask().Intervals();
        const int *intervalsD = intervals.Pointer();
        ssize_t howMany = intervals.n();
        int minIndex,maxIndex,j,ij,pos = 0;
        DTMutableFloatArray values = Full.FloatData();
        float *valuesD = values.Pointer();
        DTFloatArray arr = addThis.FloatData();
        const float *arrD = arr.Pointer();
        for (j=0;j<howMany;j++) {
            minIndex = intervalsD[2*j];
            maxIndex = intervalsD[1+2*j]+1;
            for (ij=minIndex;ij<maxIndex;ij++) {
                valuesD[ij] -= arrD[pos++];
            }
        }
    }
    else if (Full.DoublePrecision() && addThis.DoublePrecision()) {
        // Just change this portion of the mesh
        DTIntArray intervals = grid.Mask().Intervals();
        const int *intervalsD = intervals.Pointer();
        ssize_t howMany = intervals.n();
        int minIndex,maxIndex,j,ij,pos = 0;
        DTMutableDoubleArray values = Full.DoubleData();
        double *valuesD = values.Pointer();
        DTDoubleArray arr = addThis.DoubleData();
        const double *arrD = arr.Pointer();
        for (j=0;j<howMany;j++) {
            minIndex = intervalsD[2*j];
            maxIndex = intervalsD[1+2*j]+1;
            for (ij=minIndex;ij<maxIndex;ij++) {
                valuesD[ij] -= arrD[pos++];
            }
        }
    }}

void operator+=(DTMutableMesh3D &Full,const DTMesh3DBanded &addThis)
{
    if (Full.Grid()!=addThis.Grid()) {
        DTErrorMessage("DTMutableMesh3D+=DTMesh3DBand","Incompatible grids");
        return;
    }

    DTMesh3DGrid grid = Full.Grid();

    // Just change this portion of the mesh
    DTIntArray intervals = grid.Mask().Intervals();
    const int *intervalsD = intervals.Pointer();
    ssize_t howMany = intervals.n();
    int minIndex,maxIndex,j,ij,pos = 0;
    DTMutableFloatArray values = Full.FloatData();
    float *valuesD = values.Pointer();
    DTFloatArray arr = addThis.FloatData();
    const float *arrD = arr.Pointer();
    for (j=0;j<howMany;j++) {
        minIndex = intervalsD[2*j];
        maxIndex = intervalsD[1+2*j]+1;
        for (ij=minIndex;ij<maxIndex;ij++) {
            valuesD[ij] += arrD[pos++];
        }
    }
}

void operator*=(DTMutableMesh3DBanded &banded,const DTMesh3D &Full)
{
    if (SameExceptForMask(Full.Grid(),banded.Grid())==false) {
        DTErrorMessage("DTMutableMesh3DBanded*=DTMesh3D","Incompatible grids");
        return;
    }

    DTMesh3DGrid grid = banded.Grid();

    // Just change this portion of the mesh
    DTIntArray intervals = grid.Mask().Intervals();
    const int *intervalsD = intervals.Pointer();
    ssize_t howMany = intervals.n();
    int minIndex,maxIndex,j,ij,pos = 0;
    DTFloatArray values = Full.FloatData();
    const float *valuesD = values.Pointer();
    DTMutableFloatArray arr = banded.FloatData();
    float *arrD = arr.Pointer();
    for (j=0;j<howMany;j++) {
        minIndex = intervalsD[2*j];
        maxIndex = intervalsD[1+2*j]+1;
        for (ij=minIndex;ij<maxIndex;ij++) {
            arrD[pos++] *= valuesD[ij];
        }
    }
}

void CopyValues(DTMutableMesh3DBanded &A,const DTMesh3D &B)
{
	if (B.Grid().MaskDefined()) {
		if (A.Grid()!=B.Grid()) {
			DTErrorMessage("CopyValues(DTMesh3DBanded,DTMesh3D)","Grid needs to be the same");
			return;
		}
	}
	else {
		if (SameExceptForMask(A.Grid(),B.Grid())==false) {  // The size has to match
			DTErrorMessage("CopyValues(DTMesh3DBanded,DTMesh3D)","Not defined on the same region of space");
			return;
		}
	}
	if (B.DoublePrecision()!=A.DoublePrecision()) {
		DTErrorMessage("CopyValues(DTMesh3DBanded,DTMesh3D)","The meshes need to have the same precision.");
		return;
	}
	if (A.FloatPrecision()==false) {
		DTErrorMessage("CopyValues(DTMesh3DBanded,DTMesh3D)","Not implemented for double yet.");
		return;
    }
    
	DTIntArray intervals = A.Grid().Mask().Intervals();
    const int *intervalsD = intervals.Pointer();
    DTMutableFloatArray values = A.FloatData();
	float *valuesD = values.Pointer();
    DTFloatArray arr = B.FloatData();
    const float *arrD = arr.Pointer();
    
    ssize_t howMany = intervals.n();
    int minIndex,maxIndex,j,ij,pos = 0;
    for (j=0;j<howMany;j++) {
        minIndex = intervalsD[2*j];
        maxIndex = intervalsD[1+2*j]+1;
        for (ij=minIndex;ij<maxIndex;ij++) {
            valuesD[pos++] = arrD[ij];
        }
    }
}

DTMesh3D ConvertToFull(const DTMesh3DBanded &A)
{
	DTMesh3DGrid grid = A.Grid();
	ssize_t m = grid.m();
	ssize_t n = grid.n();
	ssize_t o = grid.o();
	DTMutableFloatArray toReturn(m,n,o);
	toReturn = 0;
	DTMask theMask = grid.Mask();
	DTIntArray intervals = theMask.Intervals();
	ssize_t howManyIntervals = intervals.n();
	int start,end,which;
	DTFloatArray theValues = A.FloatData();
	const float *fromD = theValues.Pointer();
	float *toD = toReturn.Pointer();
	
	int pos = 0;
	for (which=0;which<howManyIntervals;which++) {
		start = intervals(0,which);
		end = intervals(1,which);
		std::memcpy(toD+start,fromD+pos,(end-start+1)*sizeof(float));
		pos += end-start+1;
	}
	
    return DTMesh3D(grid,toReturn);
}

DTMutableMesh3DBanded ConvertToBanded(const DTMesh3D &A)
{
    if (A.Grid().MaskDefined()==false) {
        DTErrorMessage("ConvertToBanded(DTMesh3D)","The mesh needs to have a mask defined");
        return DTMutableMesh3DBanded();
    }
    DTIntArray intervals = A.Grid().Mask().Intervals();
    const int *intervalsD = intervals.Pointer();
    ssize_t j,howMany = intervals.n();
    int minIndex,maxIndex,ijk,pos = 0;
    DTMutableFloatArray values(A.Grid().NumberOfPointsInMask());
    float *valuesD = values.Pointer();
    DTFloatArray arr = A.FloatData();
    const float *arrD = arr.Pointer();
    for (j=0;j<howMany;j++) {
        minIndex = intervalsD[2*j];
        maxIndex = intervalsD[1+2*j]+1;
        for (ijk=minIndex;ijk<maxIndex;ijk++) {
            valuesD[pos++] = arrD[ijk];
        }
    }

    return DTMutableMesh3DBanded(A.Grid(),values);
}

DTMutableMesh3DBanded ConvertToBanded(const DTMesh3D &A,double outside)
{
    DTMesh3DGrid grid = A.Grid();
    ssize_t m = grid.m();
    ssize_t n = grid.n();
    ssize_t o = grid.o();
    
    if (A.DoublePrecision()) {
        DTErrorMessage("ConvertToBanded","Not done yet for double arrays");
        return DTMutableMesh3DBanded();
    }

    
    DTMutableIntArray startIndices(n*o+1);
    DTMutableShortIntArray intervals(2,1000);
    ssize_t length = intervals.n();
    int pos = 0;
    ssize_t i,j,k,start,posInValues,lengthOfValues;

    float outV = float(outside);
    DTFloatArray values = A.FloatData();
    const float *valuesD = values.Pointer();
    lengthOfValues = 1000;
    DTMutableFloatArray newValues(1000);
    posInValues = 0;
    ssize_t ijk_start = 0;
    const float *valuesDStart;

    for (k=0;k<o;k++) {
        for (j=0;j<n;j++) {
            startIndices(j+k*n) = pos;
            i = 0;
            valuesDStart = valuesD+ijk_start;
            while (i<m) {
                // Find the first point inside
                while (i<m && fabsf(valuesDStart[i])==outV) i++;
                start = i;
                while (i<m && fabsf(valuesDStart[i])!=outV) i++;
                if (start<i) {
                    if (pos==length) {
                        intervals = IncreaseSize(intervals,intervals.Length());
                        length = intervals.n();
                    }
                    if (posInValues+(i-start)>=lengthOfValues) {
                        newValues = IncreaseSize(newValues,lengthOfValues);
                        lengthOfValues = newValues.Length();
                    }
                    intervals(0,pos) = short(start);
                    intervals(1,pos) = short(i);
                    pos++;
                    memcpy(newValues.Pointer()+posInValues,valuesDStart+start,(i-start)*4);
                    posInValues+=(i-start);
                }
            }
            
            ijk_start += m;
        }
    }
    startIndices(n*o) = pos;

    intervals = TruncateSize(intervals,2*pos);
    newValues = TruncateSize(newValues,posInValues);
    DTMesh3DGrid newGrid = ApplyMask(grid,DTMask3D(startIndices,intervals,m,n,o));
    return DTMutableMesh3DBanded(newGrid,newValues);

    /*
    DTMutableIntArray intervals(2,100);
    int length = intervals.n();
    int pos = 0;
    int j,k,ijk,until,start,posInValues,lengthOfValues;
    
    if (A.DoublePrecision()) {
        return DTMutableMesh3DBanded();
    }
    else {
        float outV = outside;
        DTFloatArray values = A.FloatData();
        const float *valuesD = values.Pointer();
        lengthOfValues = 1000;
        DTMutableFloatArray newValues(1000);
        posInValues = 0;
        for (k=0;k<o;k++) {
            for (j=0;j<n;j++) {
                ijk = j*m + k*m*n;
                until = ijk+m;
                while (ijk<until) {
                    // Find the first point inside
                    while (ijk<until && fabsf(valuesD[ijk])==outV) ijk++;
                    start = ijk;
                    while (ijk<until && fabsf(valuesD[ijk])!=outV) ijk++;
                    if (start<ijk) {
                        if (pos==length) {
                            intervals = IncreaseSize(intervals,intervals.Length());
                            length = intervals.n();
                        }
                        if (posInValues+(ijk-start)>=lengthOfValues) {
                            newValues = IncreaseSize(newValues,lengthOfValues);
                            lengthOfValues = newValues.Length();
                        }
                        intervals(0,pos) = start;
                        intervals(1,pos) = ijk-1;
                        pos++;
                        memcpy(newValues.Pointer()+posInValues,valuesD+start,(ijk-start)*4);
                        posInValues+=(ijk-start);
                    }
                }
            }
        }
        intervals = TruncateSize(intervals,2*pos);
        newValues = TruncateSize(newValues,posInValues);
        DTMesh3DGrid newGrid = ApplyMask(grid,DTMask(intervals,m,n,o));
        return DTMutableMesh3DBanded(newGrid,newValues);
    }
     */
}

extern DTRegion3D BoundingBox(const DTMesh3DBanded &M)
{
    return BoundingBox(M.Grid());
}

DTRegion1D ValueRange(const DTMesh3DBanded &M)
{
    return ValueRange(M.FloatData());
}

DTMesh3DBanded Crop(const DTMesh3DBanded &mesh,const DTRegion3D &crop)
{
    DTMesh3DGrid grid = mesh.Grid();
    
    // DTIntArray intervals = grid.Mask().Intervals();
    // int howMany = int(intervals.n());

    int m = int(mesh.m());
    int n = int(mesh.n());
    int o = int(mesh.o());
    DTPoint3D minPoint = grid.ConvertCoordinateToGrid(DTPoint3D(crop.xmin,crop.ymin,crop.zmin));
    if (minPoint.x<0) minPoint.x = 0;
    if (minPoint.y<0) minPoint.y = 0;
    if (minPoint.z<0) minPoint.z = 0;
    short iMin = short(ceil(minPoint.x-1e-10));
    int jMin = int(ceil(minPoint.y-1e-10));
    int kMin = int(ceil(minPoint.z-1e-10));
    DTPoint3D maxPoint = grid.ConvertCoordinateToGrid(DTPoint3D(crop.xmax,crop.ymax,crop.zmax));
    if (maxPoint.x>=m) maxPoint.x = m-1;
    if (maxPoint.y>=n) maxPoint.y = n-1;
    if (maxPoint.z>=o) maxPoint.z = o-1;
    short iMax = short(floor(maxPoint.x+1e-10))+1;
    int jMax = int(floor(maxPoint.y+1e-10))+1;
    int kMax = int(floor(maxPoint.z+1e-10))+1;
    if (iMin>=m || jMin>=n || kMin>=o || iMax<0 || jMax<0 || kMax<0)
        return DTMesh3DBanded();
    
    DTMask3D mask = grid.Mask3D();
    DTIntArray starts = mask.Starts();
    DTIntArray startValues = mask.StartOfData();
    DTShortIntArray intervals = mask.Intervals();
    
    // Find an upper bound on how many intervals we need
    int upperBoundIntervals = 0;
    int upperBoundValues = 0;
    int j,k;
    for (k=kMin;k<kMax;k++) {
        upperBoundIntervals += starts(jMax+k*n) - starts(jMin + k*n);
        upperBoundValues += startValues(jMax+k*n) - startValues(jMin + k*n);
    }
    
    DTMutableShortIntArray newIntervals(2,upperBoundIntervals);
    DTMutableFloatArray newValues(upperBoundValues);
    float *newValuesD = newValues.Pointer();
    DTMutableIntArray newStarts((jMax-jMin)*(kMax-kMin)+1);

    DTFloatArray values = mesh.FloatData();
    const float *valuesD = values.Pointer();
    int posInValues = 0;
    int posNewValues = 0;

    int interval,endAt;
    int jkNew = 0;
    int posInNewIntervals = 0;
    int startI,endI;
    for (k=kMin;k<kMax;k++) {
        for (j=jMin;j<jMax;j++) {
            // All of the intervals for (:,j,k)
            newStarts(jkNew) = posInNewIntervals;
            interval = starts(j+k*n);
            endAt = starts(j+k*n+1);
            posInValues = startValues(j+k*n);
            // Find the first interval that ends after or at the iMin value
            while (interval<endAt && intervals(1,interval)<=iMin) {
                posInValues += intervals(1,interval) - intervals(0,interval);
                interval++;
            }
            while (interval<endAt && intervals(0,interval)<iMax) {
                // Know that iMin<intervals(1,interval) and interval(0,interval)<iMax
                // This interval overlaps with [iMin,iMax-1]
                startI = std::max(iMin,intervals(0,interval));
                // if iMin<=intervals(0,interval) then iMin <= startI = intervals(0,interval)<int(1,interval)<iMax
                // if iMin>intervals(0,interval) then iMin <= startI = iMin<iMax.  That means that
                // iMin<=startI<iMax
                
                endI = std::min(iMax,intervals(1,interval));
                // if iMax<=intervals(1,interval) then endI = iMax
                // if iMax>intervals(1,interval) then endI = intervals(1,interval)<iMax.  That means that
                // endI<=iMax
                
                // What is missing is that startI<endI.  Let's go through the four cases
                // iMin<=intervals(0,interval) && iMax<=intervals(1,interval)
                // startI = intervals(0,interval) < iMax = endI  - check
                // iMin<=intervals(0,interval) && iMax>intervals(1,interval)
                // startI = intervals(0,interval) < intervals(1,interval) = endI - check
                // iMin>intervals(0,interval) && iMax<=intervals(1,interval)
                // startI = iMin < iMax = endI  - check
                // iMin>intervals(0,interval) && iMax>intervals(1,interval)
                // startI = iMin < intervals(1,interval) = endI - check
                newIntervals(0,posInNewIntervals) = short(startI - iMin);
                newIntervals(1,posInNewIntervals) = short(endI - iMin);
                posInNewIntervals++;
                
                // Debugging
                if (posNewValues+(endI-startI)>upperBoundValues) DTErrorMessage("Out of bounds");
                
                // Copy the values over
                memcpy(newValuesD+posNewValues,valuesD + posInValues + (startI-intervals(0,interval)), (endI-startI)*sizeof(float));
                posNewValues += endI-startI;
                posInValues += intervals(1,interval) - intervals(0,interval);
                interval++;
            }
            jkNew++;
        }
    }
    newStarts(jkNew++) = posInNewIntervals;
    
    if (posInNewIntervals<upperBoundIntervals) {
        newIntervals = TruncateSize(newIntervals, 2*posInNewIntervals);
    }
    if (posNewValues<upperBoundValues) {
        newValues = TruncateSize(newValues,posNewValues);
    }
    
    DTMask3D newMask(newStarts,newIntervals,iMax-iMin,jMax-jMin,kMax-kMin);
    DTMesh3DGrid newGrid(grid.Origin() + DTPoint3D(iMin*grid.dx(),jMin*grid.dy(),kMin*grid.dz()),grid.dx(),grid.dy(),grid.dz(),newMask.m(),newMask.n(),newMask.o());
    newGrid = ApplyMask(newGrid,newMask);
    return DTMesh3DBanded(newGrid,newValues);
    
    /*

    for (index=0;index<howMany;index++) {
        ijk = intervals(0,index);
        iStart = ijk%m;
        j = (ijk/m)%n;
        k = (ijk/mn);
        iEnd = intervals(1,index)%m;
        if (j<jMin || j>jMax || k<kMin || k>kMax || iEnd<iMin || iStart>iMax) {
            posInOldValues += iEnd-iStart+1;
        }
        else {
            // Figure out the new limits for the i portion, and copy the data over
            shift = -iMin + (j-jMin)*mNew + (k-kMin)*mnNew;
            startI = std::max(iStart,iMin);
            endI = std::min(iEnd,iMax);
            newIntervals(0,posInIntervals) = startI + shift;
            newIntervals(1,posInIntervals) = endI + shift;
            posInIntervals++;
            startIndex = posInOldValues + startI-iStart; // iStart is the start of the interval, startI is where the new values start.
            for (ijk=startI;ijk<=endI;ijk++) {
                newValues(posInValues++) = values(startIndex++);
            }
            posInOldValues += iEnd-iStart+1;
        }
    }

    newValues = TruncateSize(newValues,posInValues);
    newIntervals = TruncateSize(newIntervals,2*posInIntervals);

    DTMesh3DGrid newGrid(grid.Origin() + DTPoint3D(iMin*grid.dx(),jMin*grid.dy(),kMin*grid.dz()),grid.dx(),grid.dy(),grid.dz(),mNew,nNew,oNew);
    newGrid = ApplyMask(newGrid,DTMask(newIntervals,mNew,nNew,oNew));

    return DTMesh3DBanded(newGrid,newValues);
     */
}

void Read(const DTDataStorage &input,const std::string &name,DTMesh3DBanded &toReturn)
{
    DTMesh3DGrid grid;
    Read(input,name+"_loc",grid);

    DTMask3D mask;
    Read(input,name+"_dom",mask);
    grid = ChangeSize(grid,mask.m(),mask.n(),mask.o());
    grid = ApplyMask(grid,mask);

    if (grid.IsEmpty()) return;
    if (input.SavedAsDouble(name)) {
        DTDoubleArray values;
        Read(input, name, values);
        if (values.Length()!=grid.NumberOfPointsInMask()) {
            DTErrorMessage("Read(file,name,Mesh3DBanded)", "Invalid length");
        }
        else {
            toReturn = DTMesh3DBanded(grid,values);
        }
    }
    else {
        DTFloatArray values;
        Read(input, name, values);
        if (values.Length()!=grid.NumberOfPointsInMask()) {
            DTErrorMessage("Read(file,name,Mesh3DBanded)", "Invalid length");
        }
        else {
            toReturn = DTMesh3DBanded(grid,values);
        }
    }
}

void WriteFloat(DTDataStorage &output,const std::string &name,const DTMesh3DBanded &theMesh)
{
    Write(output,name+"_bbox3D",BoundingBox(theMesh));
    WriteNoSize(output,name+"_loc",theMesh.Grid());
    Write(output,name+"_dom",theMesh.Grid().Mask3D());
    if (theMesh.FloatPrecision())
        Write(output,name,theMesh.FloatData());
    else
        Write(output,name,ConvertToFloat(theMesh.DoubleData()));
}

void Write(DTDataStorage &output,const std::string &name,const DTMesh3DBanded &theMesh)
{
    Write(output,name+"_bbox3D",BoundingBox(theMesh));
    WriteNoSize(output,name+"_loc",theMesh.Grid());
    Write(output,name+"_dom",theMesh.Grid().Mask3D());
    if (theMesh.FloatPrecision())
        Write(output,name,theMesh.FloatData());
    else
        Write(output,name,theMesh.DoubleData());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh3DBanded &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Mesh");
    output.Flush();
}


