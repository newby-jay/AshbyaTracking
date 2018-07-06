// Part of DTSource. Copyright 2006. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTTransform2D.h"
#include "DTVector2D.h"
#include "DTError.h"
#include "DTDoubleArray.h"
#include "DTFloatArray.h"


DTTransform2D::DTTransform2D()
{
    _T11 = 1.0; _T12 = 0.0; _S1 = 0.0;
    _T21 = 0.0; _T22 = 1.0; _S2 = 0.0;
    _Px = 0; _Py = 0;
}

DTTransform2D::DTTransform2D(double t11,double t12,double s1,
                             double t21,double t22,double s2)
{
    _T11 = t11; _T12 = t12; _S1 = s1;
    _T21 = t21; _T22 = t22; _S2 = s2;
    _Px = 0; _Py = 0;
}

DTTransform2D::DTTransform2D(double t11,double t12,double s1,
                             double t21,double t22,double s2,
                             double px,double py)
{
    _T11 = t11; _T12 = t12; _S1 = s1;
    _T21 = t21; _T22 = t22; _S2 = s2;
    _Px = px; _Py = py;
}

void DTTransform2D::pinfo() const
{
    std::cerr << _T11 << ", " << _T12 << ", " << _S1 << "\n";
    std::cerr << _T21 << ", " << _T22 << ", " << _S2 << "\n";
    std::cerr << _Px << ", " << _Py << ", 1\n";
}

double DTTransform2D::operator()(int i,int j) const
{
    if (i<0 || j<0 || i>2 || j>2) {
        DTErrorMessage("DTTransform::operator(i,j)","Index out of bounds");
        return 0.0;
    }
    
    double toReturn = 0.0;
    switch (i+j*3) {
        case 0:  toReturn = _T11; break;
        case 1:  toReturn = _T21; break;
        case 2:  toReturn = _Px; break;
        case 3:  toReturn = _T12; break;
        case 4:  toReturn = _T22; break;
        case 5:  toReturn = _Py; break;
        case 6: toReturn =  _S1; break;
        case 7: toReturn =  _S2; break;
        case 8: toReturn = 1.0; break;
        default: toReturn = 0.0; break;
    }
    
    return toReturn;
}

DTPoint2D DTTransform2D::operator*(const DTPoint2D &P) const
{
    if (IsPerspective()) {
        double scaleBy = 1.0/(_Px*P.x + _Py*P.y + 1);
        return DTPoint2D((_T11*P.x+_T12*P.y + _S1)*scaleBy,
                         (_T21*P.x+_T22*P.y + _S2)*scaleBy);
    }
    else {
        return DTPoint2D(_T11*P.x+_T12*P.y+_S1,
                         _T21*P.x+_T22*P.y+_S2);
        
    }
}

DTVector2D DTTransform2D::operator*(const DTVector2D &P) const
{
    if (IsPerspective()) {
        double scaleBy = 1.0/(_Px*P.x + _Py*P.y + 1);
        return DTVector2D((_T11*P.x+_T12*P.y + _S1)*scaleBy,
                         (_T21*P.x+_T22*P.y + _S2)*scaleBy);
    }
    else {
        return DTVector2D(_T11*P.x+_T12*P.y+_S1,
                          _T21*P.x+_T22*P.y+_S2);
    }
}

DTTransform2D DTTransform2D::operator*(const DTTransform2D &T) const
{
    if (T.IsPerspective() && IsPerspective()) {
        DTErrorMessage("Transform2D*Transform2D","Not possible when both have perspective");
        return DTTransform2D(); // Identity
    }
    else if (T.IsPerspective()) {
        // this*T and this matrix does not have perspective
        // The first mapping is
        // xh = (x*T.T11 + y*T.T12 + T.S1)/(1+T.Px*x + T.Py*y)
        // Then this gets mapped by (xh*T11 + yh*T12+ S1) (this mapping)
        // Which means that the end result (x component) is
        // ((x*T.T11 + y*T.T12 + T.S1)/(1+T.Px*x + T.Py*y)*T11 + (x*T.T21 + y*T.T22 + T.S2)/(1+T.Px*x + T.Py*y)*T12 + S1)
        // = (x*(T11*T.T11 + T12*T.T21 + S1*T.Px) + y*(T11*T.T12 + T12*T.22 + S1*T.Py) + S1 + T11*T.S1 + T12*T.S2)/(1+T.Px*x + T.Py*y)
        // Similarly the y component is
        // ((x*T.T11 + y*T.T12 + T.S1)/(1+T.Px*x + T.Py*y)*T21 + (x*T.T21 + y*T.T22 + T.S2)/(1+T.Px*x + T.Py*y)*T22 + S2)
        // = (x*(T21*T.T11 + T22*T.T21 + S1*T.Px) + y*(T21*T.T12 + T22*T.22 + S2*T.Py) + S2 + T21*T.S1 + T22*T.S2)/(1+T.Px*x + T.Py*y)

        return DTTransform2D((_T11*T._T11 + _T12*T._T21 + _S1*T._Px),(_T11*T._T12 + _T12*T._T22 + _S1*T._Py),_S1 + _T11*T._S1 + _T12*T._S2,
                             (_T21*T._T11 + _T22*T._T21 + _S1*T._Px),(_T21*T._T12 + _T22*T._T22 + _S2*T._Py),_S2 + _T21*T._S1 + _T22*T._S2,
                             T._Px,T._Py);
        
    }
    else if (IsPerspective()) {
        // this is perspective, but T is not
        // First mapping is
        // xh = x*T.T11 + y*T.T12 + T.S1
        // Then this gets mapped by the current matrix, so the first component is
        // (T11*(x*T.T11 + y*T.T12 + T.S1) + T12*(x*T.T21 + y*T.T22 + T.S2) + S1)/(Px*(x*T.T11 + y*T.T12 + T.S1) + Py*(x*T.T21 + y*T.T22 + T.S2) + 1)
        // = ((T11*T.T11 + T12*T.T21)*x + (T11*T.T12 + T12*T.T22)*y + (T11*T.S1 + T12*T.S2 + S1))/(x*(Px*T.T11 + Py*T21) + y*(Px*T.T12 Py*T.T22) + (Px*T.S1 + Py*T.S2 + 1))
        // Define Dx = Px*T.S1 + Py*T.S2 + 1, and divide both the nominator and the denominator with this
        double D = _Px*T._S1 + _Py*T._S2 + 1;
        return DTTransform2D((_T11*T._T11 + _T12*T._T21)/D,(_T11*T._T12 + _T12*T._T22)/D,(_T11*T._S1 + _T12*T._S2 + _S1)/D,
                             (_T21*T._T11 + _T22*T._T21)/D,(_T21*T._T12 + _T22*T._T22)/D,(_T21*T._S1 + _T22*T._S2 + _S2)/D,
                             (_Px*T._T11 + _Py*T._T21)/D,(_Px*T._T12 + _Py*T._T22)/D);
    }
    else {
        return DTTransform2D(_T11*T._T11+_T12*T._T21,
                             _T11*T._T12+_T12*T._T22,
                             _T11*T._S1 +_T12*T._S2+_S1,
                             _T21*T._T11+_T22*T._T21,
                             _T21*T._T12+_T22*T._T22,
                             _T21*T._S1 +_T22*T._S2+_S2);
    }
}

bool DTTransform2D::IsOrthogonal() const
{
    if (fabs(_T11*_T12 + _T21*_T22)<1e-15 && _Px==0 && _Py==0) {
        double n1 = _T11*_T11 + _T21*_T21;
        double n2 = _T12*_T12 + _T22*_T22;
        return (fabs(n1-1.0)<1e-14 && fabs(n2-1.0)<1e-14);
    }
    else {
        return false;
    }
}

DTTransform2D DTTransform2D::Inverse() const
{
    // A*x + b = y
    // So x = inv(A)*(y-b) = inv(A)*y - inv(A)*b
    
    if (IsPerspective()) {
        // scale = Px*x + Py*y+1;
        // newX = (T11*x + T12*y + S1)/scale;
        // newY = (T21*x + T22*y + S2)/scale;
        
        // To compute the inverse, i.e. if you hand in newX,newY find x,y
        // newX*(Px*x + Py*y+1) = T11*x + T12*y + S1, and similar for Y
        // (newX*Px-T11)*x + (newX*Py-T12)*y = S1-newX
        // (newY*Px-T21)*x + (newY*Py-T22)*y = S2-newY
        // If this matrix is [Aij]*[x,y]' = [S1-newX,S2-newY]', then [x,y]' is
        // given by [A22 -A12;-A21 A11]*[S1-newX,S2-newY]/detA where
        
        // detA =   (newX*Px-T11)*(newY*Py-T22) - (newX*Py-T12)*(newY*Px-T21)
        //      =   (newX*Px-T11)*newY*Py - (newX*Px-T11)*T22
        //        - (newX*Py-T12)*newY*Px + (newX*Py-T12)*T21
        //      = - T11*newY*Py + T12*newY*Px - newX*Px*T22 + T11*T22 + newX*Py*T21 - T12*T21
        //      = newX*(Py*T21 - Px*T22) + newY*(T12*Px - T11*Py) + (T11*T22 - T12*T21)
        //      = DivideByTransform*detT, where DivideByTransform = (newPx*newX + newPy*newY+1)
        
        
        double detT = _T11*_T22-_T12*_T21;
        double newPx = (_Py*_T21 - _Px*_T22)/detT;
        double newPy = (_T12*_Px - _T11*_Py)/detT;
        
        // The transformation portion should be DivideByTransform*x,DivideByTransform*y
        // so that the scaling will give x,y.  This means that the transformation should give
        // x = ( A22*(S1-newX) - A12*(S2-newY))/detT
        // y = (-A21*(S1-newX) + A11*(S2-newY))/detT
        // Aij will depend on newX and newY so it becomes a matrix*[newX;newY] + shift
        // x = ( A22*(S1-newX) - A12*(S2-newY))/detT
        //   = ((newY*Py-T22)*(S1-newX) - (newX*Py-T12)*(S2-newY))/detT
        //   = (  newY*Py*S1 - newY*Py*newX - T22*S1 + T22*newX
        //      - newX*Py*S2 + newX*Py*newY + T12*S2 - T12*newY)/detT
        //   = newX*(T22 - Py*S2)/detT + newY*(Py*S1 - T12)/detT + (T12*S2-T22*S1)/detT
        // y = (-A21*(S1-newX) + A11*(S2-newY))/detT
        //   = (- (newY*Px-T21)*(S1-newX) + (newX*Px-T11)*(S2-newY))/detT
        //   = (- newY*Px*S1 + newY*Px*newX + T21*S1 - T21*newX
        //   =  + newX*Px*S2 - newX*Px*newY - T11*S2 + T11*newY)/detT
        //   = newX*(Px*S2-T21)/detT + newY*(T11 - Px*S1)/detT + (T21*S1 - T11*S2)/detT
        
        double newT11 = (_T22 - _Py*_S2)/detT;
        double newT12 = (_Py*_S1 - _T12)/detT;
        double newT21 = (_Px*_S2-_T21)/detT;
        double newT22 = (_T11 - _Px*_S1)/detT;
        double newS1 = (-_T22*_S1 + _T12*_S2)/detT;
        double newS2 = (_T21*_S1-_T11*_S2)/detT;
        
        return DTTransform2D(newT11,newT12,newS1,
                             newT21,newT22,newS2,
                             newPx,newPy);
    }
    else {
        double overallDet = _T11*_T22 - _T12*_T21;
        
        double t11 =  _T22/overallDet;
        double t21 = -_T21/overallDet;
        
        double t12 = -_T12/overallDet;
        double t22 = _T11/overallDet;
        
        double s1 = - (t11*_S1 + t12*_S2);
        double s2 = - (t21*_S1 + t22*_S2);
        
        return DTTransform2D(t11,t12,s1,
                             t21,t22,s2);
    }
}

DTTransform2D DTTransform2D::Shift(const DTPoint2D &p)
{
    return DTTransform2D(1.0,0.0,p.x,
                         0.0,1.0,p.y);
}

DTTransform2D DTTransform2D::Scale(double xs,double ys)
{
    return DTTransform2D(xs,0.0,0.0,
                         0.0,ys,0.0);
}

DTTransform2D Inverse(const DTTransform2D &T)
{
    return T.Inverse();
}

DTTransform2D TransformFromCoordinateToGrid(const DTPoint2D &origin,double dx,double dy)
{
    // i -> i*dx + xzero
    return DTTransform2D(dx, 0, origin.x,
                         0, dy, origin.y);
}

DTTransform2D TransformFromGridToCoordinate(const DTPoint2D &origin,double dx,double dy)
{
    // x -> (x-xzero)/dx
    return DTTransform2D(1.0/dx,     0, -origin.x/dx,
                         0,     1.0/dy, -origin.y/dx);
}

DTMutableDoubleArray TransformPoints(const DTTransform2D &T,const DTDoubleArray &A)
{
    if (A.IsEmpty()) return DTMutableDoubleArray();
    if (A.m()!=2 || A.o()!=1) {
        DTErrorMessage("TransformPoints(Transform2D,DoubleArray","Invalid array size");
        return DTMutableDoubleArray();
    }
    
    size_t i,howMany = A.n();
    DTMutableDoubleArray toReturn(2,howMany);
    double x,y;
    
    double T11 = T(0,0);
    double T21 = T(1,0);
    double T12 = T(0,1);
    double T22 = T(1,1);
    double S1 = T(0,2);
    double S2 = T(1,2);
    
    if (T.IsPerspective()) {
        double scaleBy;
        double Px = T(2,0);
        double Py = T(2,1);
        
        for (i=0;i<howMany;i++) {
            x = A(0,i);
            y = A(1,i);
            scaleBy = 1.0/(Px*x + Py*y + 1);
            toReturn(0,i) = (T11*x+T12*y + S1)*scaleBy;
            toReturn(1,i) = (T21*x+T22*y + S2)*scaleBy;
        }
    }
    else {
        for (i=0;i<howMany;i++) {
            x = A(0,i);
            y = A(1,i);
            toReturn(0,i) = T11*x+T12*y + S1;
            toReturn(1,i) = T21*x+T22*y + S2;
        }
    }
    
    return toReturn;
}

void Read(const DTDataStorage &input,const std::string &name,DTTransform2D &toReturn)
{
    DTDoubleArray theArray = input.ReadDoubleArray(name);
    if (theArray.Length()==0) {
        toReturn = DTTransform2D();
    }
    else if (theArray.m()==2 && theArray.n()==3 && theArray.o()==1) {
        toReturn = DTTransform2D(theArray(0,0),theArray(0,1),theArray(0,2),
                                 theArray(1,0),theArray(1,1),theArray(1,2));
    }
    else if (theArray.m()==3 && theArray.n()==3 && theArray.o()==1) {
        toReturn = DTTransform2D(theArray(0,0),theArray(0,1),theArray(0,2),
                                 theArray(1,0),theArray(1,1),theArray(1,2),
                                 theArray(2,0),theArray(2,1));
    }
    else {
        DTErrorMessage("ReadFromArray(Transform2D)","Invalid size for array.");
        toReturn = DTTransform2D();
    }
}

void Write(DTDataStorage &output,const std::string &name,const DTTransform2D &theVar)
{
    if (theVar.IsPerspective()) {
        DTMutableDoubleArray theArr(3,3);
        for (int j=0;j<3;j++) {
            for (int i=0;i<3;i++) {
                theArr(i,j) = theVar(i,j);
            }
        }
        output.Save(theArr,name);
    }
    else {
        DTMutableDoubleArray theArr(2,3);
        for (int j=0;j<3;j++) {
            for (int i=0;i<2;i++) {
                theArr(i,j) = theVar(i,j);
            }
        }
        output.Save(theArr,name);
    }
}

void WriteOne(DTDataStorage &output,const std::string &name,const DTTransform2D &toWrite)
{
    Write(output,name,toWrite);
    Write(output,"Seq_"+name,"2D Transform");
    output.Flush();
}

