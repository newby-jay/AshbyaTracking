// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTVectorField3D.h"

#include "DTError.h"
#include "DTRegion3D.h"

DTVectorField3D::DTVectorField3D(const DTMesh3DGrid &gr,const DTFloatArray &x,const DTFloatArray &y,const DTFloatArray &z)
{
    // Check that the sizes are compatible.
    if (x.m()!=y.m() || x.n()!=y.n() || x.o()!=y.o() || x.m()!=z.m() || x.n()!=z.n() || x.o()!=z.o()) {
        DTErrorMessage("DTVectorField3D(Grid,X,Y,Z)","Incompatible sizes.");
        return;
    }
    if ((gr.m() && x.m()!=gr.m()) || (gr.n()!=0 && x.n()!=gr.n()) || (gr.o()!=0 && x.o()!=gr.o())) {
        DTErrorMessage("DTVectorField3D(Grid,X,Y,Z)","Incompatible sizes.");
        return;
    }

	grid = gr;
    if (gr.m()!=x.m() || gr.n()!=x.n() || gr.o()!=x.o()) {
        grid = ChangeSize(gr,x.m(),x.n(),x.o());
    }
	
    xComponent = x;
    yComponent = y;
    zComponent = z;
}

void DTVectorField3D::pinfo(void) const
{
    grid.pinfo();
}

DTVectorField3D ApplyMask(const DTVectorField3D &field,const DTMask &mask)
{
	if (field.m()==mask.m() && field.n()==mask.n() && field.o()==mask.o()) {
		return DTVectorField3D(ApplyMask(field.Grid(),mask),field.X(),field.Y(),field.Z());
	}
	else {
		DTErrorMessage("ApplyMask(DTVectorField3D,DTMask)","Incompatible mask size.");
		return field;
	}
}

DTVectorField3D RemoveMask(const DTVectorField3D &field)
{
    return DTVectorField3D(RemoveMask(field.Grid()),field.X(),field.Y(),field.Z());
}

DTRegion3D BoundingBox(const DTVectorField3D &var)
{
    return BoundingBox(var.Grid());
}

float MaximumLength(const DTVectorField3D &field)
{
    DTFloatArray X = field.X();
    DTFloatArray Y = field.Y();
    DTFloatArray Z = field.Z();
    ssize_t mno = X.Length();
    ssize_t ijk;
    const float *XD = X.Pointer();
    const float *YD = Y.Pointer();
    const float *ZD = Z.Pointer();
    float toReturn = 0.0;
    float len;
    for (ijk=0;ijk<mno;ijk++) {
        len = XD[ijk]*XD[ijk] + YD[ijk]*YD[ijk] + ZD[ijk]*ZD[ijk];
        if (isfinite(len) && len>toReturn) {
            toReturn = len;
        }
    }
    return sqrtf(toReturn);
}

void Read(const DTDataStorage &input,const std::string &name,DTVectorField3D &toReturn)
{
    DTMesh3DGrid grid;
    Read(input,name,grid);
    if (grid.IsEmpty()) {
        toReturn = DTVectorField3D();
        return;
    }

    DTFloatArray X,Y,Z;
    Read(input,name+"_X",X);
    Read(input,name+"_Y",Y);
    Read(input,name+"_Z",Z);
    
    std::string domName = name+"_dom";
    if (input.Contains(domName)) {
        DTMask mask;
        Read(input,domName,mask);
        grid = ApplyMask(grid,mask);
    }

    toReturn = DTVectorField3D(grid,X,Y,Z);
}

void Write(DTDataStorage &output,const std::string &name,const DTVectorField3D &saveThis)
{
    Write(output,name+"_bbox3D",BoundingBox(saveThis));

    Write(output,name+"_X",saveThis.X());
    Write(output,name+"_Y",saveThis.Y());
    Write(output,name+"_Z",saveThis.Z());

    if (saveThis.Grid().MaskDefined()) {
        Write(output,name+"_dom",saveThis.Grid().Mask());
    }
    
    WriteNoSize(output,name,saveThis.Grid());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTVectorField3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Vector Field");
    output.Flush();
}

