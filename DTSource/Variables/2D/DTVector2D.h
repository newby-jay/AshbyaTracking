// Part of DTSource. Copyright 2004-2015. David A. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTVector2D_H
#define DTVector2D_H

struct DTVector2D {
    DTVector2D() {}
    DTVector2D(double xv,double yv) : x(xv), y(yv) {}

    double x,y;
};

inline bool operator==(const DTVector2D &A,const DTVector2D &B) {return (A.x==B.x && A.y==B.y);}
inline bool operator!=(const DTVector2D &A,const DTVector2D &B) {return (A.x!=B.x || A.y!=B.y);}

inline DTVector2D operator+(const DTVector2D &A,const DTVector2D &B) {return DTVector2D(A.x+B.x,A.y+B.y);}
inline DTVector2D operator-(const DTVector2D &A,const DTVector2D &B) {return DTVector2D(A.x-B.x,A.y-B.y);}
inline DTVector2D operator*(const DTVector2D &A,double v) {return DTVector2D(A.x*v,A.y*v);}
inline DTVector2D operator*(double v,const DTVector2D &A) {return DTVector2D(A.x*v,A.y*v);}
inline DTVector2D operator/(const DTVector2D &A,double v) {return DTVector2D(A.x/v,A.y/v);}
inline DTVector2D operator-(const DTVector2D &A) {return DTVector2D(-A.x,-A.y);}

inline double Dot(const DTVector2D &A,const DTVector2D &B) {return (A.x*B.x+A.y*B.y);}

#endif
