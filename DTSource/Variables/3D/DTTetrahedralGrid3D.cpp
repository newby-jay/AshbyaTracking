// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTTetrahedralGrid3D.h"
#include "DTError.h"
#include "DTRegion1D.h"
#include "DTRegion3D.h"
#include "DTUtilities.h"

DTTetrahedralGrid3D::DTTetrahedralGrid3D(const DTIntArray &OffsetsIn,const DTPointCollection3D &PointsIn)
{
    if (PointsIn.IsEmpty() && OffsetsIn.IsEmpty())
        return;

    if (OffsetsIn.m()!=4 || OffsetsIn.o()!=1) {
        DTErrorMessage("DTTetrahedralGrid3D(offsets,points)","The offset array has incorrect size.");
        return;
    }

    DTRegion1D offRange = ValueRange(OffsetsIn);
    if (offRange.minV<0 || offRange.maxV>=PointsIn.NumberOfPoints()) {
        DTErrorMessage("DTTetrahedralGrid3D(offsets,points)","Offset array refers to points out of range.");
        return;
    }

    points = PointsIn;
    offsets = OffsetsIn;
}

DTTetrahedralGrid3D::DTTetrahedralGrid3D(const DTIntArray &OffsetsIn,const DTPointCollection3D &PointsIn,
                                         const DTIntArray &NextTetra,const DTIntArray &PointNumbersIn)
{
    if (PointsIn.IsEmpty() && OffsetsIn.IsEmpty())
        return;

    if (OffsetsIn.m()!=4 || OffsetsIn.o()!=1) {
        DTErrorMessage("DTTetrahedralGrid3D(offsets,points,tetra,points)",
                       "The offset array has incorrect size.");
        return;
    }

    DTRegion1D offRange = ValueRange(OffsetsIn);
    if (offRange.minV<0 || offRange.maxV>=PointsIn.NumberOfPoints()) {
        DTErrorMessage("DTTetrahedralGrid3D(offsets,points,tetra,points)",
                       "Offset array refers to points out of range.");
        return;
    }

    if (NextTetra.NotEmpty()) {
        if (NextTetra.m()!=4 || NextTetra.n()!=OffsetsIn.n() || NextTetra.o()!=1) {
            DTErrorMessage("DTTetrahedralGrid3D(offsets,points,tetra,points)",
                           "tetra array has incorrect size.");
            return;    
        }
        offRange = ValueRange(NextTetra);
        if (offRange.minV<-1 || offRange.maxV>=4*OffsetsIn.n()) {
            DTErrorMessage("DTTetrahedralGrid3D(offsets,points,tetra,points)",
                           "Offset array refers to tetrahedra out of range.");
            return;
        }
    }

    if (PointNumbersIn.NotEmpty()) {
        if (PointNumbersIn.m()!=PointsIn.NumberOfPoints() || PointNumbersIn.n()!=1 || PointNumbersIn.o()!=1) {
            DTErrorMessage("DTTetrahedralGrid3D(offsets,points,tetra,points)",
                           "point number array has incorrect size.");
            return;
        }
    }

    points = PointsIn;
    offsets = OffsetsIn;
    nextTetra = NextTetra;
    pointNumbers = PointNumbersIn;
}

DTTetrahedralGrid3D::DTTetrahedralGrid3D(const DTIntArray &OffsetsIn,const DTPointCollection3D &PointsIn,const DTIntArray &NextTetra)
{
    if (PointsIn.IsEmpty() && OffsetsIn.IsEmpty())
        return;
    
    if (OffsetsIn.m()!=4 || OffsetsIn.o()!=1) {
        DTErrorMessage("DTTetrahedralGrid3D(offsets,points,tetra)",
                       "The offset array has incorrect size.");
        return;
    }
    
    DTRegion1D offRange = ValueRange(OffsetsIn);
    if (offRange.minV<0 || offRange.maxV>=PointsIn.NumberOfPoints()) {
        DTErrorMessage("DTTetrahedralGrid3D(offsets,points,tetra)",
                       "Offset array refers to points out of range.");
        return;
    }
    
    if (NextTetra.NotEmpty()) {
        if (NextTetra.m()!=4 || NextTetra.n()!=OffsetsIn.n() || NextTetra.o()!=1) {
            DTErrorMessage("DTTetrahedralGrid3D(offsets,points,tetra)",
                           "tetra array has incorrect size.");
            return;    
        }
        offRange = ValueRange(NextTetra);
        if (offRange.minV<-1 || offRange.maxV>=4*OffsetsIn.n()) {
            DTErrorMessage("DTTetrahedralGrid3D(offsets,points,tetra)",
                           "Offset array refers to tetrahedra out of range.");
            return;
        }
    }
    
    points = PointsIn;
    offsets = OffsetsIn;
    nextTetra = NextTetra;
}

bool DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid(void) const
{
    // Check the following:

    // Indices have to lie between 0 and 4*TetraCount-1
    
    ssize_t numberOfTetra = nextTetra.n();
    int j,offset,tetra,side,A,B,C,D,rA,rB,rC,rD;
    bool isValid = true;
    
    for (j=0;j<numberOfTetra;j++) {
        A = offsets(0,j);
        B = offsets(1,j);
        C = offsets(2,j);
        D = offsets(3,j);

        // First treat ABC on this tetra
        offset = nextTetra(0,j);

        if (offset!=-1) {
            tetra = offset/4;
            side = offset%4;
            
            // Tetrahedra needs to be valid.
            if (tetra<0 || tetra>=numberOfTetra) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side ABC points to a tetrahedra that is out of bounds");
                break;
            }
            
            if (tetra==j) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side ABC points back to the same tetrahedra.");
                break;
            }
        
            if (nextTetra(side,tetra)!=4*j) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side on the adjoining triangle does not point back to this tetrahedra");
                break;
            }
            
            if (tetra>j) {
                // The other tetra needs to point back to this one.
                // Make sure that the points on the other tetrahedra aligns with the ABC side.  Note that the orientation needs to be reversed
                // 0 = ABC, 1 = ADB, 2 = ACD, 3 = BDC
                rA = offsets(0,tetra);
                rB = offsets(1,tetra);
                rC = offsets(2,tetra);
                rD = offsets(3,tetra);
                
                switch (side) {
                    case 0:
                        // Connects to ABC on the other tetrahedra
                        isValid = ((A==rA && B==rC && C==rB) || (A==rB && B==rA && C==rC) || (A==rC && B==rB && C==rA));
                        break;
                    case 1:
                        // Connects to ADB on the other tetrahedra
                        isValid = ((A==rA && B==rB && C==rD) || (A==rD && B==rA && C==rB) || (A==rB && B==rD && C==rA));
                        break;
                    case 2:
                        // Connects to ACD on the other tetrahedra
                        isValid = ((A==rA && B==rD && C==rC) || (A==rC && B==rA && C==rD) || (A==rD && B==rC && C==rA));
                        break;
                    case 3:
                        // Connects to BDC on the other tetrahedra
                        isValid = ((A==rB && B==rC && C==rD) || (A==rD && B==rB && C==rC) || (A==rC && B==rD && C==rB));
                        break;
                }
                
                if (!isValid) {
                    DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side ABC points to a remote tetrahedra that doesn't point back correctly.");
                    break;
                }
            }
        }
        
        
        // ADB on this tetra
        offset = nextTetra(1,j);
        
        if (offset!=-1) {
            tetra = offset/4;
            side = offset%4;
            
            // Tetrahedra needs to be valid.
            if (tetra<0 || tetra>=numberOfTetra) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side ADB points to a tetrahedra that is out of bounds");
                break;
            }
            
            if (tetra==j) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side ADB points back to the same tetrahedra.");
                break;
            }
            
            if (nextTetra(side,tetra)!=4*j+1) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side on the adjoining triangle does not point back to this tetrahedra");
                break;
            }
            
            if (tetra>j) {
                // The other tetra needs to point back to this one.
                // Make sure that the points on the other tetrahedra aligns with the ABC side.  Note that the orientation needs to be reversed
                // 0 = ABC, 1 = ADB, 2 = ACD, 3 = BDC
                rA = offsets(0,tetra);
                rB = offsets(1,tetra);
                rC = offsets(2,tetra);
                rD = offsets(3,tetra);
                
                switch (side) {
                    case 0:
                        // Connects to ABC on the other tetrahedra
                        isValid = ((A==rA && D==rC && B==rB) || (A==rB && D==rA && B==rC) || (A==rC && D==rB && B==rA));
                        break;
                    case 1:
                        // Connects to ADB on the other tetrahedra
                        isValid = ((A==rA && D==rB && B==rD) || (A==rD && D==rA && B==rB) || (A==rB && D==rD && B==rA));
                        break;
                    case 2:
                        // Connects to ACD on the other tetrahedra
                        isValid = ((A==rA && D==rD && B==rC) || (A==rC && D==rA && B==rD) || (A==rD && D==rC && B==rA));
                        break;
                    case 3:
                        // Connects to BDC on the other tetrahedra
                        isValid = ((A==rB && D==rC && B==rD) || (A==rD && D==rB && B==rC) || (A==rC && D==rD && B==rB));
                        break;
                }
                
                if (!isValid) {
                    DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side ADB points to a remote tetrahedra that doesn't point back correctly.");
                    break;
                }
            }
        }
        
        
        // ACD on this tetra
        offset = nextTetra(2,j);
        
        if (offset!=-1) {
            tetra = offset/4;
            side = offset%4;
            
            // Tetrahedra needs to be valid.
            if (tetra<0 || tetra>=numberOfTetra) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side ADB points to a tetrahedra that is out of bounds");
                break;
            }
            
            if (tetra==j) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side ADB points back to the same tetrahedra.");
                break;
            }
            
            if (nextTetra(side,tetra)!=4*j+2) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side on the adjoining triangle does not point back to this tetrahedra");
                break;
            }
            
            if (tetra>j) {
                // The other tetra needs to point back to this one.
                // Make sure that the points on the other tetrahedra aligns with the ABC side.  Note that the orientation needs to be reversed
                // 0 = ABC, 1 = ADB, 2 = ACD, 3 = BDC
                rA = offsets(0,tetra);
                rB = offsets(1,tetra);
                rC = offsets(2,tetra);
                rD = offsets(3,tetra);
                
                switch (side) {
                    case 0:
                        // Connects to ABC on the other tetrahedra
                        isValid = ((A==rA && C==rC && D==rB) || (A==rB && C==rA && D==rC) || (A==rC && C==rB && D==rA));
                        break;
                    case 1:
                        // Connects to ADB on the other tetrahedra
                        isValid = ((A==rA && C==rB && D==rD) || (A==rD && C==rA && D==rB) || (A==rB && C==rD && D==rA));
                        break;
                    case 2:
                        // Connects to ACD on the other tetrahedra
                        isValid = ((A==rA && C==rD && D==rC) || (A==rC && C==rA && D==rD) || (A==rD && C==rC && D==rA));
                        break;
                    case 3:
                        // Connects to BDC on the other tetrahedra
                        isValid = ((A==rB && C==rC && D==rD) || (A==rD && C==rB && D==rC) || (A==rC && C==rD && D==rB));
                        break;
                }
                
                if (!isValid) {
                    DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side ACD points to a remote tetrahedra that doesn't point back correctly.");
                    break;
                }
            }
        }
        
        
        // BDC on this tetra
        offset = nextTetra(3,j);
        
        if (offset!=-1) {
            tetra = offset/4;
            side = offset%4;
            
            // Tetrahedra needs to be valid.
            if (tetra<0 || tetra>=numberOfTetra) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side BDC points to a tetrahedra that is out of bounds");
                break;
            }
            
            if (tetra==j) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side BDC points back to the same tetrahedra.");
                break;
            }
            
            if (nextTetra(side,tetra)!=4*j+3) {
                DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side on the adjoining triangle does not point back to this tetrahedra");
                break;
            }
            
            if (tetra>j) {
                // The other tetra needs to point back to this one.
                // Make sure that the points on the other tetrahedra aligns with the ABC side.  Note that the orientation needs to be reversed
                // 0 = ABC, 1 = ADB, 2 = ACD, 3 = BDC
                rA = offsets(0,tetra);
                rB = offsets(1,tetra);
                rC = offsets(2,tetra);
                rD = offsets(3,tetra);
                
                switch (side) {
                    case 0:
                        // Connects to ABC on the other tetrahedra
                        isValid = ((B==rA && D==rC && C==rB) || (B==rB && D==rA && C==rC) || (B==rC && D==rB && C==rA));
                        break;
                    case 1:
                        // Connects to ADB on the other tetrahedra
                        isValid = ((B==rA && D==rB && C==rD) || (B==rD && D==rA && C==rB) || (B==rB && D==rD && C==rA));
                        break;
                    case 2:
                        // Connects to ACD on the other tetrahedra
                        isValid = ((B==rA && D==rD && C==rC) || (B==rC && D==rA && C==rD) || (B==rD && D==rC && C==rA));
                        break;
                    case 3:
                        // Connects to BDC on the other tetrahedra
                        isValid = ((B==rB && D==rC && C==rD) || (B==rD && D==rB && C==rC) || (B==rC && D==rD && C==rB));
                        break;
                }
                
                if (!isValid) {
                    DTErrorMessage("DTTetrahedralGrid3D::CheckIfNextTetraArrayIsValid","Error in tetra #" + DTInt2String(j) +": Side BDC points to a remote tetrahedra that doesn't point back correctly.");
                    break;
                }
            }
        }
        
    }

    return (j==numberOfTetra);
}

DTTetrahedralGrid3D ConvertToFloat(const DTTetrahedralGrid3D &g)
{
    if (g.Points().FloatPrecision()) {
        return g;
    }
    else {
        return DTTetrahedralGrid3D(g.Offsets(),ConvertToFloat(g.Points()));
    }
}

DTTetrahedralGrid3D ConvertToDouble(const DTTetrahedralGrid3D &g)
{
    if (g.Points().DoublePrecision()) {
        return g;
    }
    else {
        return DTTetrahedralGrid3D(g.Offsets(),ConvertToDouble(g.Points()));
    }
}

void DTTetrahedralGrid3D::pinfo(void) const
{
    if (points.IsEmpty())
        std::cerr << "Empty\n" << flush;
    else
        std::cerr << points.NumberOfPoints() << " points, " << offsets.n() << " tetrahedra.\n" << flush;
}

bool operator==(const DTTetrahedralGrid3D &A,const DTTetrahedralGrid3D &B)
{
    return (A.Points()==B.Points() && A.Offsets()==B.Offsets());
}

bool operator!=(const DTTetrahedralGrid3D &A,const DTTetrahedralGrid3D &B)
{
    return (A.Points()!=B.Points() || A.Offsets()!=B.Offsets());
}

DTRegion3D BoundingBox(const DTTetrahedralGrid3D &G)
{
    return BoundingBox(G.Points());
}

struct DTTetrahedralWallsLong {
    
    bool operator<(const DTTetrahedralWallsLong &A) const {return (label<A.label);}
    
    unsigned long long int label;
    int positionInOffset;
};

struct DTTetrahedralWallsLongAndInt {
    
    bool operator<(const DTTetrahedralWallsLongAndInt &A) const {
        return (labelHigh<A.labelHigh || (labelHigh==A.labelHigh && labelLow<A.labelLow));
    }
    
    unsigned long long int labelLow;
    int labelHigh;
    int positionInOffset;
};

DTTetrahedralGrid3D AddNextTetrahedra(const DTTetrahedralGrid3D &grid)
{
    if (grid.NextTetrahedraExists()) return grid;
    
    DTIntArray offsets = grid.Offsets();
    
    int len = offsets.n();
    int i;
    int Ai,Bi,Ci,Di,ind1,ind2,ind3,temp;
    
    DTMutableIntArray NextTetra;
    
    if (len<2642245) {
        // Can save the offset as a single unsigned long long.
        DTMutableList<DTTetrahedralWallsLong> offsetInfo(4*len);
        DTTetrahedralWallsLong wall;
        
        for (i=0;i<len;i++) {
            Ai = offsets(0,i);
            Bi = offsets(1,i);
            Ci = offsets(2,i);
            Di = offsets(3,i);
            
            // Numbering is ABC, ABD, ACD, BCD
            
            // The ABC triangle.
            ind1 = Ai;
            ind2 = Bi;
            ind3 = Ci;
            // Sort the indices with bubble sort.
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            if (ind3>ind2) {temp = ind2; ind2 = ind3; ind3 = temp;}
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            wall.label = ind3;
            wall.label *= len;
            wall.label += ind2;
            wall.label *= len;
            wall.label += ind1;
            wall.positionInOffset = 4*i;
            offsetInfo(4*i) = wall;
            
            // ABD
            ind1 = Ai;
            ind2 = Bi;
            ind3 = Di;
            // Sort the indices with bubble sort.
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            if (ind3>ind2) {temp = ind2; ind2 = ind3; ind3 = temp;}
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            wall.label = ind3;
            wall.label *= len;
            wall.label += ind2;
            wall.label *= len;
            wall.label += ind1;
            wall.positionInOffset = 4*i+1;
            offsetInfo(4*i+1) = wall;
            
            // ACD
            ind1 = Ai;
            ind2 = Ci;
            ind3 = Di;
            // Sort the indices with bubble sort.
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            if (ind3>ind2) {temp = ind2; ind2 = ind3; ind3 = temp;}
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            wall.label = ind3;
            wall.label *= len;
            wall.label += ind2;
            wall.label *= len;
            wall.label += ind1;
            wall.positionInOffset = 4*i+2;
            offsetInfo(4*i+2) = wall;
            
            // BCD
            ind1 = Bi;
            ind2 = Ci;
            ind3 = Di;
            // Sort the indices with bubble sort.
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            if (ind3>ind2) {temp = ind2; ind2 = ind3; ind3 = temp;}
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            wall.label = ind3;
            wall.label *= len;
            wall.label += ind2;
            wall.label *= len;
            wall.label += ind1;
            wall.positionInOffset = 4*i+3;
            offsetInfo(4*i+3) = wall;
        }
        
        // Sort the list
        sort(offsetInfo.Pointer(),offsetInfo.Pointer()+offsetInfo.Length());
        
        // Each side should be repeated.  Go through the sorted list and connect the pairs.
        
        NextTetra = DTMutableIntArray(4,len);
        NextTetra = -1;
        
        int pos = 0;
        int pos1,pos2;
        
        while (pos<4*len-1) {
            if (offsetInfo(pos).label==offsetInfo(pos+1).label) {
                pos1 = offsetInfo(pos).positionInOffset;
                pos2 = offsetInfo(pos+1).positionInOffset;
                NextTetra(pos1%4,pos1/4) = pos2;
                NextTetra(pos2%4,pos2/4) = pos1;
                pos+=2;
            }
            else {
                // Not connected
                pos++;
            }
        }
    }
    else {
        // Problematic, since the maximum index will exceed the capacity of 64 bit integers.
        // Still possible to do, by separating the number into a ind1+len*ind2 (8 byte) and ind3 (4 bytes)
        DTMutableList<DTTetrahedralWallsLongAndInt> offsetInfo(4*len);
        DTTetrahedralWallsLongAndInt wall;
        
        for (i=0;i<len;i++) {
            Ai = offsets(0,i);
            Bi = offsets(1,i);
            Ci = offsets(2,i);
            Di = offsets(3,i);
            
            // Numbering is ABC, ABD, ACD, BCD
            
            // The ABC triangle.
            ind1 = Ai;
            ind2 = Bi;
            ind3 = Ci;
            // Sort the indices with bubble sort.
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            if (ind3>ind2) {temp = ind2; ind2 = ind3; ind3 = temp;}
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            wall.labelHigh = ind3;
            wall.labelLow = ind2;
            wall.labelLow *= len;
            wall.labelLow += ind1;
            wall.positionInOffset = 4*i;
            offsetInfo(4*i) = wall;
            
            // ABD
            ind1 = Ai;
            ind2 = Bi;
            ind3 = Di;
            // Sort the indices with bubble sort.
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            if (ind3>ind2) {temp = ind2; ind2 = ind3; ind3 = temp;}
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            wall.labelHigh = ind3;
            wall.labelLow = ind2;
            wall.labelLow *= len;
            wall.labelLow += ind1;
            wall.positionInOffset = 4*i+1;
            offsetInfo(4*i+1) = wall;
            
            // ACD
            ind1 = Ai;
            ind2 = Ci;
            ind3 = Di;
            // Sort the indices with bubble sort.
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            if (ind3>ind2) {temp = ind2; ind2 = ind3; ind3 = temp;}
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            wall.labelHigh = ind3;
            wall.labelLow = ind2;
            wall.labelLow *= len;
            wall.labelLow += ind1;
            wall.positionInOffset = 4*i+2;
            offsetInfo(4*i+2) = wall;
            
            // BCD
            ind1 = Bi;
            ind2 = Ci;
            ind3 = Di;
            // Sort the indices with bubble sort.
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            if (ind3>ind2) {temp = ind2; ind2 = ind3; ind3 = temp;}
            if (ind2>ind1) {temp = ind1; ind1 = ind2; ind2 = temp;}
            wall.labelHigh = ind3;
            wall.labelLow = ind2;
            wall.labelLow *= len;
            wall.labelLow += ind1;
            wall.positionInOffset = 4*i+3;
            offsetInfo(4*i+3) = wall;
        }
        
        // Sort the list
        sort(offsetInfo.Pointer(),offsetInfo.Pointer()+offsetInfo.Length());
        
        // Each side should be repeated.  Go through the sorted list and connect the pairs.
        
        NextTetra = DTMutableIntArray(4,len);
        NextTetra = -1;
        
        int pos = 0;
        int pos1,pos2;
        
        while (pos<4*len-1) {
            if (offsetInfo(pos).labelHigh==offsetInfo(pos+1).labelHigh &&
                offsetInfo(pos).labelLow==offsetInfo(pos+1).labelLow) {
                pos1 = offsetInfo(pos).positionInOffset;
                pos2 = offsetInfo(pos+1).positionInOffset;
                NextTetra(pos1%4,pos1/4) = pos2;
                NextTetra(pos2%4,pos2/4) = pos1;
                pos+=2;
            }
            else {
                // Not connected
                pos++;
            }
        }
    }
    
    return SetNextTetrahedra(grid,NextTetra);
}

DTTetrahedralGrid3D SetNextTetrahedra(const DTTetrahedralGrid3D &g,const DTIntArray &a)
{
    return DTTetrahedralGrid3D(g.Offsets(),g.Points(),a,g.PointNumbers());
}

DTTetrahedralGrid3D SetPointNumbers(const DTTetrahedralGrid3D &g,const DTIntArray &a)
{
    return DTTetrahedralGrid3D(g.Offsets(),g.Points(),g.NeighboringTetrahedra(),a);
}

void Read(const DTDataStorage &input,const std::string &name,DTTetrahedralGrid3D &toReturn)
{
    std::string theName = input.ResolveName(name);

    DTIntArray offsets;
    Read(input,theName,offsets);

    DTPointCollection3D points;
    Read(input,theName+"_Pts",points);

    toReturn = DTTetrahedralGrid3D(offsets,points);

    if (input.Contains(theName+"_Next")) {
        DTIntArray NextTetra;
        Read(input,theName+"_Next",NextTetra);
        toReturn = SetNextTetrahedra(toReturn,NextTetra);
    }

    if (input.Contains(theName+"_PtN")) {
        DTIntArray PtN;
        Read(input,theName+"_PtN",PtN);
        toReturn = SetPointNumbers(toReturn,PtN);
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTTetrahedralGrid3D &toWrite)
{
    Write(output,name+"_bbox3D",BoundingBox(toWrite));

    if (toWrite.NeighboringTetrahedra().NotEmpty())
        Write(output,name+"_Next",toWrite.NeighboringTetrahedra());
    if (toWrite.PointNumbers().NotEmpty())
        Write(output,name+"_PtN",toWrite.PointNumbers());
    
    Write(output,name+"_Pts",toWrite.Points());
    Write(output,name,toWrite.Offsets());
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTTetrahedralGrid3D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"3D Tetrahedral Grid");
    output.Flush();
}

void Read(const DTDataStorage &input,const std::string &name,DTTetrahedralGrid3D &toReturn,DTTetrahedralGrid3D_SaveInfo &SaveInfo)
{
    std::string theName = input.ResolveName(name);
    if (SaveInfo.name==theName) {
        toReturn = SaveInfo.grid; // Already read this grid in.
        return;
    }
    
    Read(input,theName,toReturn);
    
    SaveInfo.grid = toReturn;
    SaveInfo.name = theName;
}

void Write(DTDataStorage &output,const std::string &name,const DTTetrahedralGrid3D &toWrite,DTTetrahedralGrid3D_SaveInfo &SaveInfo)
{
    if (SaveInfo.name.length() && SaveInfo.grid==toWrite) {
        // Just save the reference.
        Write(output,name,SaveInfo.name);
    }
    else {
        Write(output,name,toWrite);
        SaveInfo.grid = toWrite;
        SaveInfo.name = name;
    }
}

