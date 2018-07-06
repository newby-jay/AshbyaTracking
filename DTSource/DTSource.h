// DTSource is a C++ library written by David Adalsteinsson.
// Free for use in an academic code.  For non-academic, please contact david@visualdatatools.com
// Standard limits apply.  The code is provided as is, without any warranty at all, or guarantiees
// that it will work at all.  You use it at your own risk.  But if you find any mistakes, please report them.

/*

 All of the code inside the DTSource directory is intended to simplify IO between your
 C++ programs and DataTank. This code does not depend on any compiled libraries, and is portable,
 so it can be used without DataTank.
 
 Copyright belongs to Visual Data Tools, but you are free
 to use this in your programs, both academic and commerical (BSD license).
 You are free to modify the source, strip out portions of it, put it with other code etc.
 If redistributed, you need to include the copyright notices and links back to visualdatatools.com
 No payment is required.  If used in a commercial application, please acknowledge it's use.

 If you find a bug in it, or something that could/should be improved, please send
 a bug/feature request to dtsource@visualdatatools.com
 
 */

/*
 When debugging, set break points in DTError.cpp (in the Support folder.)
 See that file for further information.
 */

/*
 DTSource is constantly being improved and extended.  This header file might therefore include files
 that are currently not part of your XCode project.  Just add them to the project manually, or remove
 and add back the whole DTSource directory, stored in ~/Library/DataTank/
 */

#include "DTError.h"
#include "DTArguments.h"

#include "DTArrayConversion.h"
#include "DTCharArray.h"
#include "DTDoubleArray.h"
#include "DTFloatArray.h"
#include "DTIntArray.h"
#include "DTShortIntArray.h"
#include "DTUCharArray.h"
#include "DTUShortIntArray.h"

#include "DTCharArrayRegion.h"
#include "DTDoubleArrayOperators.h"
#include "DTDoubleArrayRegion.h"
#include "DTFloatArrayOperators.h"
#include "DTFloatArrayRegion.h"
#include "DTIntArrayOperators.h"
#include "DTIntArrayRegion.h"
#include "DTShortIntArrayRegion.h"
#include "DTUCharArrayRegion.h"
#include "DTUShortIntArrayRegion.h"

#include "DTDataFile.h"
#include "DTMatlabDataFile.h"
#include "DTEndianSwap.h"
#include "DTUtilities.h"
#include "DTList.h"
#include "DTProgress.h"

#include "DTAudio.h"
#include "DTColor.h"
#include "DTColorList.h"
#include "DTStringList.h"

#include "DTFunction1D.h"
#include "DTPlot1D.h"
#include "DTRegion1D.h"

#include "DTBitmap2D.h"
#include "DTCoordinateLine2D.h"
#include "DTFunction2D.h"
#include "DTMesh2D.h"
#include "DTMesh2DGrid.h"
#include "DTPath2D.h"
#include "DTPath2DValues.h"
#include "DTPoint2D.h"
#include "DTPoint2DValue.h"
#include "DTPointCollection2D.h"
#include "DTPointValueCollection2D.h"
#include "DTPointVector2D.h"
#include "DTRegion2D.h"
#include "DTShiftScale2D.h"
#include "DTStructuredGrid2D.h"
#include "DTStructuredMesh2D.h"
#include "DTTriangularGrid2D.h"
#include "DTTriangularMesh2D.h"
#include "DTTriangularVectorField2D.h"
#include "DTVectorCollection2D.h"
#include "DTVectorField2D.h"

#include "DTAtomCollection3D.h"
#include "DTCoordinatePlane3D.h"
#include "DTFunction3D.h"
#include "DTMesh3D.h"
#include "DTMesh3DGrid.h"
#include "DTPath3D.h"
#include "DTPath3DValues.h"
#include "DTPlane3D.h"
#include "DTPoint3D.h"
#include "DTPoint3DValue.h"
#include "DTPointCollection3D.h"
#include "DTPointValueCollection3D.h"
#include "DTPointVector2D.h"
#include "DTPrismGrid3D.h"
#include "DTPrismMesh3D.h"
#include "DTPrismVectorField3D.h"
#include "DTRegion3D.h"
#include "DTShiftScale3D.h"
#include "DTStructuredGrid3D.h"
#include "DTStructuredMesh3D.h"
#include "DTSurface3D.h"
#include "DTSurfaceValues3D.h"
#include "DTTetrahedralMesh3D.h"
#include "DTTetrahedralVectorField3D.h"
#include "DTVector3D.h"
#include "DTVectorCollection3D.h"
#include "DTVectorField3D.h"

