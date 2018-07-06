//
//  Hypha.cpp
//  meshSkel
//
//  Created by Jay Newby on 10/26/17.
//
//

#include "Hypha.h"


void HyphaGraph::WriteStructure(DTDataStorage &output,string name)
{
    output.Save("Ncaps", name+"_1N");
    output.Save("Real Number", name+"_1T");
    output.Save("Nbranches", name+"_2N");
    output.Save("Real Number", name+"_2T");
    output.Save("NskelPoints", name+"_3N");
    output.Save("Real Number", name+"_3T");
    output.Save("skelGraph", name+"_4N");
    output.Save("Array", name+"_4T");
    output.Save("z", name+"_5N");
    output.Save("Array", name+"_5T");
    output.Save("caps", name+"_6N");
    output.Save("Array", name+"_6T");
    output.Save("branches", name+"_7N");
    output.Save("Array", name+"_7T");
    output.Save("branchGraph", name+"_8N");
    output.Save("Array", name+"_8T");
    
    output.Save(8,name+"_N");
    output.Save("Group",name);
}

void Read(DTDataStorage &input,string name,HyphaGraph &var)
{
    Read(input,name+"_Ncaps",var.Ncaps);
    Read(input,name+"_Nbranches",var.Nbranches);
    Read(input,name+"_NskelPoints",var.NskelPoints);
    Read(input,name+"_skelGraph",var.skelGraph);
    Read(input,name+"_z",var.z);
    Read(input,name+"_caps",var.caps);
    Read(input,name+"_branches",var.branches);
    Read(input,name+"_branchGraph",var.branchGraph);
}

void Write(DTDataStorage &output,string name,const HyphaGraph &var)
{
    Write(output,name+"_Ncaps",var.Ncaps);
    Write(output,name+"_Nbranches",var.Nbranches);
    Write(output,name+"_NskelPoints",var.NskelPoints);
    Write(output,name+"_skelGraph",var.skelGraph);
    Write(output,name+"_z",var.z);
    Write(output,name+"_caps",var.caps);
    Write(output,name+"_branches",var.branches);
    Write(output,name+"_branchGraph",var.branchGraph);
    
    Write(output,name,DTDoubleArray()); // So that DataTank can see the variable.
}

void Tracks::WriteStructure(DTDataStorage &output,string name)
{
    output.Save("points", name+"_1N");
    output.Save("PointCollection3D", name+"_1T");
    output.Save("pointToParticle", name+"_2N");
    output.Save("Array", name+"_2T");
    output.Save("particleToPoints", name+"_3N");
    output.Save("Array", name+"_3T");
    output.Save("radius", name+"_4N");
    output.Save("Array", name+"_4T");
    output.Save("time", name+"_5N");
    output.Save("Array", name+"_5T");
    output.Save("skelAndSurfInd", name+"_6N");
    output.Save("Array", name+"_6T");
    output.Save("minDistToSurf", name+"_7N");
    output.Save("Array", name+"_7T");
    
    output.Save(7,name+"_N");
    output.Save("Group",name);
}

void Read(DTDataStorage &input,string name,Tracks &var)
{
    Read(input,name+"_points",var.points);
    Read(input,name+"_pointToParticle",var.pointToParticle);
    Read(input,name+"_particleToPoints",var.particleToPoints);
    Read(input,name+"_radius",var.radius);
    Read(input,name+"_time",var.time);
    Read(input,name+"_skelAndSurfInd",var.skelAndSurfInd);
    Read(input,name+"_minDistToSurf",var.minDistToSurf);
}

void Write(DTDataStorage &output,string name,const Tracks &var)
{
    Write(output,name+"_points",var.points);
    Write(output,name+"_pointToParticle",var.pointToParticle);
    Write(output,name+"_particleToPoints",var.particleToPoints);
    Write(output,name+"_radius",var.radius);
    Write(output,name+"_time",var.time);
    Write(output,name+"_skelAndSurfInd",var.skelAndSurfInd);
    Write(output,name+"_minDistToSurf",var.minDistToSurf);
    
    Write(output,name,DTDoubleArray()); // So that DataTank can see the variable.
}
void Hypha::WriteStructure(DTDataStorage &output,string name)
{
    output.Save("surface", name+"_1N");
    output.Save("Surface3D", name+"_1T");
    output.Save("surfToSkeleton", name+"_2N");
    output.Save("Array", name+"_2T");
    output.Save("skelToSurf", name+"_3N");
    output.Save("Array", name+"_3T");
    output.Save("skelToSurfIndex", name+"_4N");
    output.Save("Array", name+"_4T");
    output.Save("skeleton", name+"_5N");
    output.Save("PointCollection3D", name+"_5T");
    output.Save("GEMS", name+"_6N");
    Tracks::WriteStructure(output,name+"_6T");
    output.Save("nuclei", name+"_7N");
    Tracks::WriteStructure(output,name+"_7T");
    output.Save("hyphaGraph", name+"_8N");
    HyphaGraph::WriteStructure(output,name+"_8T");
    output.Save("DS", name+"_9N");
    output.Save("Array", name+"_9T");
    output.Save("branchLengths", name+"_10N");
    output.Save("Array", name+"_10T");
    output.Save("maxRadius", name+"_11N");
    output.Save("Real Number", name+"_11T");
    
    output.Save(11,name+"_N");
    output.Save("Group",name);
}

void Read(DTDataStorage &input,string name,Hypha &var)
{
    Read(input,name+"_surface",var.surface);
    Read(input,name+"_surfToSkeleton",var.surfToSkeleton);
    Read(input,name+"_skelToSurf",var.skelToSurf);
    Read(input,name+"_skelToSurfIndex",var.skelToSurfIndex);
    Read(input,name+"_skeleton",var.skeleton);
    Read(input,name+"_GEMS",var.GEMS);
    Read(input,name+"_nuclei",var.nuclei);
    Read(input,name+"_hyphaGraph",var.hyphaGraph);
    Read(input,name+"_DS",var.DS);
    Read(input,name+"_branchLengths",var.branchLengths);
    Read(input,name+"_maxRadius",var.maxRadius);
}

void Write(DTDataStorage &output,string name,const Hypha &var)
{
    Write(output,name+"_surface",var.surface);
    Write(output,name+"_surfToSkeleton",var.surfToSkeleton);
    Write(output,name+"_skelToSurf",var.skelToSurf);
    Write(output,name+"_skelToSurfIndex",var.skelToSurfIndex);
    Write(output,name+"_skeleton",var.skeleton);
    Write(output,name+"_GEMS",var.GEMS);
    Write(output,name+"_nuclei",var.nuclei);
    Write(output,name+"_hyphaGraph",var.hyphaGraph);
    Write(output,name+"_DS",var.DS);
    Write(output,name+"_branchLengths",var.branchLengths);
    Write(output,name+"_maxRadius",var.maxRadius);
    
    Write(output,name,DTDoubleArray()); // So that DataTank can see the variable.
}

////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Misc Tools //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
std::list<Hypha> addTracks(const std::list<Hypha> &H, const DTDoubleArray &paths, int GEMSorNuclei)
{
    //// x, y, z, t, particle, radius
    //// 0, 1, 2, 3,   4,       5
    std::list<Hypha> toReturn;
    int seg = 1;
    int NtrackPoints = paths.m();
    int Ncols = paths.n();
    for (Hypha Hn : H) {
        HyphaGraph hg = Hn.hyphaGraph;
        int NskelPoints = Hn.skeleton.NumberOfPoints();
        std::list<int> surfInds, skelInds, pointInds, particleNumbers;
        std::list<double> dists2surf;
        for (int n=0; n<NtrackPoints; n++) {
            if ((Ncols == 7) && (paths(n, 6) != seg)) continue; // if seg data is included, use it for speed
            DTPoint3D xt(paths(n, 0), paths(n, 1), paths(n, 2));
            double minDist = 1e20, regionCheck = -1.0;
            int skelMin = -1, surfMin = -1;
            //// locate closest surface point
            for (int m=0; m<NskelPoints; m++) {
                DTPoint3D xskel = Hn.skeleton(m);
                double d2skel = Distance(xskel, xt);
                if (d2skel > Hn.maxRadius) continue;
                int NsurfPoints = Hn.skelToSurfIndex(m, 1);
                int skToSind = Hn.skelToSurfIndex(m, 0);
                for (int i=0; i<NsurfPoints; i++) {
                    int sp = Hn.skelToSurf(skToSind + i);
                    DTPoint3D xsurf = Hn.surface.Points()(sp);
                    double d2surf = Distance(xsurf, xt);
                    if (d2surf < minDist) {
                        regionCheck = Distance(xskel, xsurf) - d2skel + 2.0;
                        minDist = d2surf;
                        surfMin = sp;
                        skelMin = m;
                    }
                }
            }
            if (regionCheck < 0) continue; // point is outside hypha
            surfInds.push_back(surfMin);
            skelInds.push_back(skelMin);
            dists2surf.push_back(minDist);
            pointInds.push_back(n);
            particleNumbers.push_back(paths(n, 4));
        }
        int Nhyph = pointInds.size();
        particleNumbers.unique();
        int Nparticles = particleNumbers.size();
        DTMutableIntArray skelAndSurfInd(Nhyph, 2);
        skelAndSurfInd = -1;
        DTMutableDoubleArray minDistToSurf(Nhyph);
        DTMutableIntArray times(Nhyph);
        DTMutableDoubleArray radius(Nhyph);
        DTMutableDoubleArray pointData(3, Nhyph);
        DTMutableIntArray pointToParticle(Nhyph);
        DTMutableIntArray particleToPoints(Nparticles, 2);
        particleToPoints = 0;
        int globalPnumber = -1, localPnumber = -1;
        for (int n=0; n<Nhyph; n++) {
            int np = pointInds.front();
            pointInds.pop_front();
            skelAndSurfInd(n, 0) = skelInds.front();
            skelAndSurfInd(n, 1) = surfInds.front();
            skelInds.pop_front();
            surfInds.pop_front();
            minDistToSurf(n) = dists2surf.front();
            dists2surf.pop_front();
            times(n) = paths(np, 3);
            radius(n) = paths(np, 5);
            pointData(0, n) = paths(np, 0);
            pointData(1, n) = paths(np, 1);
            pointData(2, n) = paths(np, 2);
            if (globalPnumber != paths(np, 4)) {
                globalPnumber = paths(np, 4);
                localPnumber++;
                particleToPoints(localPnumber, 0) = n;
            }
            pointToParticle(n) = localPnumber;
            particleToPoints(localPnumber, 1) += 1;
        }
        DTPointCollection3D points(pointData);
        if (GEMSorNuclei == 0) {
            Tracks GEMS;
            GEMS.points = points;
            GEMS.radius = radius;
            GEMS.time = times;
            GEMS.pointToParticle = pointToParticle;
            GEMS.particleToPoints = particleToPoints;
            GEMS.skelAndSurfInd = skelAndSurfInd;
            GEMS.minDistToSurf = minDistToSurf;
            Hn.GEMS = GEMS;
        }
        else {
            Tracks nuclei;
            nuclei.points = points;
            nuclei.radius = radius;
            nuclei.time = times;
            nuclei.pointToParticle = pointToParticle;
            nuclei.particleToPoints = particleToPoints;
            nuclei.skelAndSurfInd = skelAndSurfInd;
            nuclei.minDistToSurf = minDistToSurf;
            Hn.nuclei = nuclei;
        }
        toReturn.push_back(Hn);
        seg++;
    }
    return toReturn;
}
DTIntArray segmentPoints(const std::list<Hypha> &H, const DTDoubleArray &points)
{
    //// x, y, z
    //// 0, 1, 2
    int NtrackPoints = points.m();
    DTMutableIntArray toReturn(NtrackPoints);
    toReturn = 0;
    int seg = 1;
    for (Hypha Hn : H) {
        HyphaGraph hg = Hn.hyphaGraph;
        int NskelPoints = Hn.skeleton.NumberOfPoints();
        for (int n=0; n<NtrackPoints; n++) {
            DTPoint3D xt(points(n, 0), points(n, 1), points(n, 2));
            double minDist = 1e20, regionCheck = -1.0;
            int skelMin = -1, surfMin = -1;
            //// locate closest surface point
            for (int m=0; m<NskelPoints; m++) {
                DTPoint3D xskel = Hn.skeleton(m);
                double d2skel = Distance(xskel, xt);
                if (d2skel > Hn.maxRadius) continue;
                int NsurfPoints = Hn.skelToSurfIndex(m, 1);
                int skToSind = Hn.skelToSurfIndex(m, 0);
                for (int i=0; i<NsurfPoints; i++) {
                    int sp = Hn.skelToSurf(skToSind + i);
                    DTPoint3D xsurf = Hn.surface.Points()(sp);
                    double d2surf = Distance(xsurf, xt);
                    if (d2surf < minDist) {
                        regionCheck = Distance(xskel, xsurf) - d2skel + 2.0;
                        minDist = d2surf;
                        surfMin = sp;
                        skelMin = m;
                    }
                }
            }
            if (regionCheck < 0) continue; // point is outside hypha
            toReturn(n) = seg;
        }
        seg++;
    }
    return toReturn;
}
std::pair<DTIntArray, DTDoubleArray> GEMSdistanceFromNuclei(const Hypha &h, double maxDist)
{
    int Nnuclei = h.nuclei.points.NumberOfPoints();
    int Ngems = h.GEMS.points.NumberOfPoints();
    std::list<double> distList;
    std::list<int> pointList;
    for (int n=0; n<Nnuclei; n++) {
        DTPoint3D xnucl = h.nuclei.points(n);
        int t = h.nuclei.time(n);
        for (int m=0; m<Ngems; m++) {
            if (h.GEMS.time(m) != t) continue;
            DTPoint3D xgem = h.GEMS.points(m);
            double d = Distance(xnucl, xgem);
            if (d > maxDist) continue;
            distList.push_back(d);
            pointList.push_back(m);
        }
    }
    int Nobs = distList.size();
    DTMutableIntArray pointNumbers(Nobs);
    DTMutableDoubleArray distances(Nobs);
    for (int n=0; n<Nobs; n++) {
        pointNumbers(n) = pointList.front();
        distances(n) = distList.front();
        distList.pop_front();
        pointList.pop_front();
    }
    std::pair<DTIntArray, DTDoubleArray> toReturn = std::make_pair(pointNumbers, distances);
    return toReturn;
}

DTDoubleArray incrementValueSet(const Tracks &tracks, const DTIntArray &pointNumbers, const DTDoubleArray &values)
{
    int N = values.m();
    DTMutableDoubleArray toReturn(N, 5);
    toReturn = NAN;
    for (int n=0; n<N; n++) {
        int npoint = pointNumbers(n);
        int nparticle = tracks.pointToParticle(npoint);
        int nFirstPoint = tracks.particleToPoints(nparticle, 0);
        int trackLength = tracks.particleToPoints(nparticle, 1);
        if ((npoint == nFirstPoint) || (trackLength == 1)) continue;
        int t0 = tracks.time(npoint-1), t1 = tracks.time(npoint);
        DTPoint3D x0 = tracks.points(npoint-1);
        DTPoint3D x1 = tracks.points(npoint);
        toReturn(n, 0) = x1.x - x0.x;
        toReturn(n, 1) = x1.y - x0.y;
        toReturn(n, 2) = x1.z - x0.z;
        toReturn(n, 3) = t1 - t0;
        toReturn(n, 4) = values(n);
    }
    return toReturn;
}

std::list<int> localSkelPoints(const Hypha &h, int skelPoint, int range)
{
    std::list<int> toReturn;
    DTIntArray G = h.hyphaGraph.skelGraph;
    toReturn.push_back(skelPoint);
    int branch = 0, branchInd = -1;
    int branchChan = -1;
    for (int channel=0; channel<=1; channel++) {
        int now = skelPoint;
        for (int n=0; n<range; n++) {
            int nxt = G(now, channel);
            if (nxt == -1) break;
            toReturn.push_back(nxt);
            if (h.hyphaGraph.z(nxt) == 3) {
                branch = 1;
                branchInd = nxt;
                if (now == G(branchInd, 0)) branchChan = 0;
                else if (now == G(branchInd, 1)) branchChan = 1;
                else if (now == G(branchInd, 2)) branchChan = 2;
                break;
            }
            now = nxt;
        }
    }
    if (branch == 1) {
        for (int Bch=0; Bch<3; Bch++) {
            if (Bch == branchChan) continue;
            int now = G(branchInd, Bch);
            toReturn.push_back(now);
            int channel = (Bch == 2) ? 1:0;
            for (int n=0; n<range; n++) {
                int nxt = G(now, channel);
                if ((nxt == -1) || (h.hyphaGraph.z(nxt) == 3)) break;
                toReturn.push_back(nxt);
                now = nxt;
            }
        }
    }
    return toReturn;
}

std::list<int> localSurfacePoints(const Hypha &h, int skelPoint, int range)
{
    std::list<int> toReturn;
    std::list<int> skelPoints = localSkelPoints(h, skelPoint, range);
    for (int n : skelPoints) {
        int NsurfPoints = h.skelToSurfIndex(n, 1);
        int skToSind = h.skelToSurfIndex(n, 0);
        for (int i=0; i<NsurfPoints; i++) {
            int sp = h.skelToSurf(skToSind + i);
            toReturn.push_back(sp);
        }
    }
    return toReturn;
}

DTDoubleArray diffusivitySurfaceMap(const Hypha &h, int window,int seg,int NF, double sigma, int range)
{
    DTPointCollection3D surfPoints = h.surface.Points();
    int Npoints = h.GEMS.points.NumberOfPoints();
    int NsurfPoints = h.surface.NumberOfPoints();
    DTMutableDoubleArray D(NF, NsurfPoints);
    DTMutableDoubleArray Vx(NF, NsurfPoints);
    DTMutableDoubleArray Vy(NF, NsurfPoints);
    DTMutableDoubleArray Vz(NF, NsurfPoints);
    DTMutableDoubleArray U(NF, NsurfPoints);
    D = 0;
    U = 0;
    Vx = 0;
    Vy = 0;
    Vz = 0;
    for (int n=0; n<Npoints; n++) {
        int nparticle = h.GEMS.pointToParticle(n);
        int nFirstPoint = h.GEMS.particleToPoints(nparticle, 0);
        int trackLength = h.GEMS.particleToPoints(nparticle, 1);
        int nLastPoint = nFirstPoint + trackLength - 1;
        if ((n == nFirstPoint) || (n == nLastPoint) || (trackLength <= 2)) continue;
        int t0 = h.GEMS.time(n-1), t1 = h.GEMS.time(n+1);
        DTPoint3D x0 = h.GEMS.points(n-1);
        DTPoint3D xn = h.GEMS.points(n);
        DTPoint3D x1 = h.GEMS.points(n+1);
        double vx = (x1.x - x0.x)/(t1 - t0);
        double vy = (x1.y - x0.y)/(t1 - t0);
        double vz = (x1.z - x0.z)/(t1 - t0);
        //double px = (2.*xn.x - x1.x - x0.x)/(t1 - t0);
        //double py = (2.*xn.y - x1.y - x0.y)/(t1 - t0);
        //double pz = (2.*xn.z - x1.z - x0.z)/(t1 - t0);
        //double sqVal = ((x1.x - x0.x)*(x1.x - x0.x) + (x1.y - x0.y)*(x1.y - x0.y))/(4.*(t1 - t0));
        double sqVal = ((x1.x - x0.x)*(x1.x - x0.x) + (x1.y - x0.y)*(x1.y - x0.y)
                         + (x1.z - x0.z)*(x1.z - x0.z))/(6.*(t1 - t0));
        int surfInd = h.GEMS.skelAndSurfInd(n, 1);
        int skelInd = h.GEMS.skelAndSurfInd(n, 0);
        DTPoint3D xsurf0 = h.surface.Points()(surfInd);
        for (int sp : localSurfacePoints(h, skelInd, range)) {
            DTPoint3D xsurf = surfPoints(sp);
            double d = Distance(xsurf0, xsurf);
            double wx = exp(-d*d/(2*sigma*sigma));
            D(t1, sp) += wx*sqVal;
            Vx(t1, sp) += wx*vx;
            Vy(t1, sp) += wx*vy;
            Vz(t1, sp) += wx*vz;
            U(t1, sp) += wx;
        }
    }
    double p = 1.0/window;
    for (int t = 1; t<NF; t++) {
        for (int n=0; n<NsurfPoints; n++) {
            D(t, n) = p*D(t, n) + (1-p)*D(t-1, n);
            Vx(t, n) = p*Vx(t, n) + (1-p)*Vx(t-1, n);
            Vy(t, n) = p*Vy(t, n) + (1-p)*Vy(t-1, n);
            Vz(t, n) = p*Vz(t, n) + (1-p)*Vz(t-1, n);
            U(t, n) = p*U(t, n) + (1-p)*U(t-1, n);
        }
    }
    double MinObservationLevel = 1e-20;
    for (int t = 1; t<NF; t++) {
        for (int n=0; n<NsurfPoints; n++) {
            if (U(t, n) < MinObservationLevel) {
                D(t, n) = NAN;
            }
            else {
                double meanSquared = (Vx(t, n)*Vx(t, n) + Vy(t, n)*Vy(t, n) + Vz(t, n)*Vz(t, n))/(3.*U(t, n)*U(t, n));
                double newVal = D(t, n)/U(t, n) - meanSquared;
                D(t, n) = (newVal > 0) ? newVal : NAN;
            }
        }
    }
    return D;
}

DTDoubleArray densitySurfaceMap(const Hypha &h, int window,int seg,int NF, double sigma, int range)
{
    DTPointCollection3D surfPoints = h.surface.Points();
    int Npoints = h.GEMS.points.NumberOfPoints();
    int NsurfPoints = h.surface.NumberOfPoints();
    DTMutableDoubleArray toReturn(NF, NsurfPoints);
    toReturn = 0;
    for (int n=0; n<Npoints; n++) {
        int t1 = h.GEMS.time(n);
        int surfInd = h.GEMS.skelAndSurfInd(n, 1);
        int skelInd = h.GEMS.skelAndSurfInd(n, 0);
        DTPoint3D xsurf0 = h.surface.Points()(surfInd);
        DTPoint3D xskel = h.skeleton(skelInd);
        double dnorm = Distance(xskel, xsurf0);
        for (int sp : localSurfacePoints(h, skelInd, range)) {
            DTPoint3D xsurf = surfPoints(sp);
            double d = Distance(xsurf0, xsurf);
            //if (d > sigma) continue;
            double wx = exp(-d*d/(2.0*sigma*sigma));
            toReturn(t1, sp) += wx/dnorm;
        }
    }
    double p = 1.0/window;
    for (int t = 1; t<NF; t++) {
        for (int n=0; n<NsurfPoints; n++) {
            toReturn(t, n) = p*toReturn(t, n) + (1-p)*toReturn(t-1, n);
        }
    }
    return toReturn;
}


std::pair<std::list<DTDoubleArray>, DTIntArray> getOrderedMedialAxis(const Hypha &h)
{
    DTPointCollection3D surfPoints = h.surface.Points();
    std::pair<std::list<DTDoubleArray>, DTIntArray> toReturn;
    /////////
    ///////// extract arrays ordered by arc length
    DTMutableIntArray skelToOrdered(h.hyphaGraph.NskelPoints);
    skelToOrdered = -1;
    //DTMutableDoubleArray arcLength(h.hyphaGraph.NskelPoints);
    std::list<DTDoubleArray> arcLengthList;
    HyphaGraph hg = h.hyphaGraph;
    
    int now = hg.caps(0);
    int nxt = hg.skelGraph(now, 0);
    int m = 0;
    std::list<double> arcLength;
    arcLength.push_back(0.0);
    while (m < h.hyphaGraph.NskelPoints) {
        double al = arcLength.back() + h.DS(now, 0);
        
        skelToOrdered(now) = m;
        if ((hg.z(nxt) == 1) || (hg.z(nxt) == 3)) {
            skelToOrdered(nxt) = m + 1;
            m++;
            break;
        }
        arcLength.push_back(al);
        now = nxt;
        nxt = hg.skelGraph(now, 0);
        m++;
    }
    DTMutableDoubleArray arcLengthFirst(arcLength.size());
    int nal = 0;
    for (double al : arcLength) {
        arcLengthFirst(nal) = al;
        nal++;
    }
    arcLengthList.push_back(arcLengthFirst);
    
    
    // gather all branches if present
    for (int n=0; n < hg.Nbranches; n++) {
        int branch = hg.branches(n);
        for (int i=0; i < 2; i++) {
            std::list<double> arcLengthBranches;
            arcLengthBranches.push_back(0.0);
            int now = branch;
            int nxt = hg.skelGraph(now, i);
            if (skelToOrdered(nxt) != -1) {
                i++;
                nxt = hg.skelGraph(now, i);
            }
            now = nxt;
            while (m < h.hyphaGraph.NskelPoints) {
                double al = arcLengthBranches.back() + h.DS(now, 0);
                skelToOrdered(now) = m;
                if ((hg.z(nxt) == 1) || (hg.z(nxt) == 3)) {
                    skelToOrdered(nxt) = m + 1;
                    m++;
                    break;
                }
                arcLengthBranches.push_back(al);
                now = nxt;
                nxt = hg.skelGraph(now, 0);
                m++;
            }
            DTMutableDoubleArray arcLengthB(arcLengthBranches.size());
            int nal = 0;
            for (double al : arcLengthBranches) {
                arcLengthB(nal) = al;
                nal++;
            }
            arcLengthList.push_back(arcLengthB);
        }
    }
    toReturn.first = arcLengthList;
    toReturn.second = skelToOrdered;
    return toReturn;
}
//std::pair<DTDoubleArray, DTIntArray> getSkelToOrdered(const Hypha &h)
//{
//    DTPointCollection3D surfPoints = h.surface.Points();
//
//    /////////
//    ///////// extract arrays ordered by arc length
//    int NbranchBreaks = 2*h.hyphaGraph.Nbranches;
//    DTMutableIntArray skelToOrdered(h.hyphaGraph.NskelPoints);
//    skelToOrdered = -1;
//    DTMutableDoubleArray arcLength(h.hyphaGraph.NskelPoints + NbranchBreaks);
//    arcLength = 0.0;
//    HyphaGraph hg = h.hyphaGraph;
//
//    int now = hg.caps(0);
//    int nxt = hg.skelGraph(now, 0);
//    int m = 0;
//    while (m < h.hyphaGraph.NskelPoints + NbranchBreaks) {
//        arcLength(m+1) = arcLength(m) + h.DS(now, 0);
//        skelToOrdered(now) = m;
//        if ((hg.z(nxt) == 1) || (hg.z(nxt) == 3)) {
//            skelToOrdered(nxt) = m + 1;
//            //m++;
//            break;
//        }
//        now = nxt;
//        nxt = hg.skelGraph(now, 0);
//        m++;
//    }
//
//    // gather all branches if present
//    for (int n=0; n < hg.Nbranches; n++) {
//        int branch = hg.branches(n);
//        double AL0 = arcLength(m);
//        for (int i=0; i < 2; i++) {
//            arcLength(m+1) = AL0;
//            m++;
//            int now = branch;
//            int nxt = hg.skelGraph(now, i);
//            while (m < h.hyphaGraph.NskelPoints + NbranchBreaks) {
//                arcLength(m+1) = arcLength(m) + h.DS(now, 0);
//                skelToOrdered(now) = m;
//                if ((hg.z(nxt) == 1) || (hg.z(nxt) == 3)) {
//                    //skelToOrdered(nxt) = m + 1;
//                    m++;
//                    break;
//                }
//                now = nxt;
//                nxt = hg.skelGraph(now, 0);
//                m++;
//            }
//        }
//    }
//    std::pair<DTDoubleArray, DTIntArray> toReturn;
//    toReturn.first = arcLength;
//    toReturn.second = skelToOrdered;
//    return toReturn;
//}

MedialAxisValues densityDiffusivityProjectedOnMedialAxis(const Hypha &h, const DTIntArray &skelToOrdered, int window, int NF, double sigma, int range)
{
    DTPointCollection3D surfPoints = h.surface.Points();
    int Npoints = h.GEMS.points.NumberOfPoints();
    DTMutableDoubleArray densityOnSkel(NF, h.hyphaGraph.NskelPoints);
    DTMutableDoubleArray diffusivityOnSkel(NF, h.hyphaGraph.NskelPoints);
    DTMutableDoubleArray U(NF, h.hyphaGraph.NskelPoints);
    DTMutableDoubleArray Vx(NF, h.hyphaGraph.NskelPoints);
    DTMutableDoubleArray Vy(NF, h.hyphaGraph.NskelPoints);
    DTMutableDoubleArray Vz(NF, h.hyphaGraph.NskelPoints);
    densityOnSkel = 0;
    diffusivityOnSkel = 0;
    U = 0;
    Vx = 0;
    Vy = 0;
    Vz = 0;
    DTMutableDoubleArray avgRadius(h.hyphaGraph.NskelPoints);
    double twopi = 6.2831853072;
    for (int n=0; n<Npoints; n++) {
        int t1 = h.GEMS.time(n);
        DTPoint3D gemPoint = h.GEMS.points(n);
        int skelInd = h.GEMS.skelAndSurfInd(n, 0);
        //// find closest medial axis point
        double dmin = 1e20;
        for (int skp : localSkelPoints(h, skelInd, range)) {
            DTPoint3D xskel = h.skeleton(skp);
            double d = Distance(gemPoint, xskel);
            if (d < dmin) {
                dmin = d;
                skelInd = skp;
            }
        }
        DTPoint3D xskel0 = h.skeleton(skelInd);
        //// for diffusivity computation
        int nparticle = h.GEMS.pointToParticle(n);
        int nFirstPoint = h.GEMS.particleToPoints(nparticle, 0);
        int trackLength = h.GEMS.particleToPoints(nparticle, 1);
        int nLastPoint = nFirstPoint + trackLength - 1;
        double obsVal = 0.;
        double vx = 0, vy = 0, vz = 0;
 
        if ((n != nFirstPoint) && (n != nLastPoint) && (trackLength > 2)) {
            int t0 = h.GEMS.time(n-1), t1 = h.GEMS.time(n+1);
            DTPoint3D x0 = h.GEMS.points(n-1);
            DTPoint3D xn = h.GEMS.points(n);
            DTPoint3D x1 = h.GEMS.points(n+1);
            vx = (x1.x - x0.x)/(t1 - t0);
            vy = (x1.y - x0.y)/(t1 - t0);
            vz = (x1.z - x0.z)/(t1 - t0);
            obsVal = ((x1.x - x0.x)*(x1.x - x0.x) + (x1.y - x0.y)*(x1.y - x0.y)
                         + (x1.z - x0.z)*(x1.z - x0.z))/(6.*(t1 - t0));
        }
        //// compute average local radius
        double radius = 0.0;
        int count = 0;
        for (int sp : localSurfacePoints(h, skelInd, 10)) {
            DTPoint3D xsurf = h.surface.Points()(sp);
            radius += Distance(xskel0, xsurf);
            count++;
        }
        radius /= count;
        avgRadius(skelInd) = radius;
        double ds = 0.5*(h.DS(skelInd, 0) + h.DS(skelInd, 1));
        double vol = twopi/2*radius*radius*ds; // washer with avarage radius and arclength thickness
        if ((h.DS(skelInd, 0) < 0) || (h.DS(skelInd, 1) < 0)) {
            vol = twopi/3.0*radius*radius*radius; // half sphere with average radius
        }
        
        for (int skp : localSkelPoints(h, skelInd, range)) {
            DTPoint3D xskel = h.skeleton(skp);
            double d = Distance(xskel0, xskel);
            if (d > 3.0*sigma) continue;
            double wx = exp(-d*d/(2.0*sigma*sigma));
            densityOnSkel(t1, skp) += ds*wx/vol/sqrt(twopi*sigma*sigma);
            if (obsVal > 0) {
                diffusivityOnSkel(t1, skp) += obsVal*wx;
                U(t1, skp) += wx;
                Vx(t1, skp) += wx*vx;
                Vy(t1, skp) += wx*vy;
                Vz(t1, skp) += wx*vz;
            }
        }
    }
    double p = 1.0/window;
    for (int t = 1; t<NF; t++) {
        for (int n=0; n<h.hyphaGraph.NskelPoints; n++) {
            densityOnSkel(t, n) = p*densityOnSkel(t, n) + (1-p)*densityOnSkel(t-1, n);
            diffusivityOnSkel(t, n) = p*diffusivityOnSkel(t, n) + (1-p)*diffusivityOnSkel(t-1, n);
            U(t, n) = p*U(t, n) + (1-p)*U(t-1, n);
            Vx(t, n) = p*Vx(t, n) + (1-p)*Vx(t-1, n);
            Vy(t, n) = p*Vy(t, n) + (1-p)*Vy(t-1, n);
            Vz(t, n) = p*Vz(t, n) + (1-p)*Vz(t-1, n);
        }
    }
    double MinObservationLevel = 1e-20;
    for (int t = 1; t<NF; t++) {
        for (int n=0; n<h.hyphaGraph.NskelPoints; n++) {
            if (U(t, n) < MinObservationLevel) {
                diffusivityOnSkel(t, n) = NAN;
            }
            else {
                double meanSquared = (Vx(t, n)*Vx(t, n) + Vy(t, n)*Vy(t, n) + Vz(t, n)*Vz(t, n))/(6.*U(t, n)*U(t, n));
                diffusivityOnSkel(t, n) = diffusivityOnSkel(t, n)/U(t, n) - meanSquared;
            }
        }
    }
    /////////
    ///////// extract arrays ordered by arc length
    //int NbranchBreaks = 2*h.hyphaGraph.Nbranches;
    DTMutableDoubleArray densityOut(NF, h.hyphaGraph.NskelPoints);
    DTMutableDoubleArray diffusivityOut(NF, h.hyphaGraph.NskelPoints);
    DTMutableDoubleArray radiusOut(h.hyphaGraph.NskelPoints);
    densityOut = NAN;
    diffusivityOut = NAN;
   
    for (int n=0;n<h.hyphaGraph.NskelPoints;n++) {
        int m = skelToOrdered(n);
        if (m < 0) continue;
        radiusOut(m) = avgRadius(n);
        for (int t=0; t<NF; t++) {
            densityOut(t, m) = densityOnSkel(t, n);
            diffusivityOut(t, m) = diffusivityOnSkel(t, n);
        }
    }
    MedialAxisValues toReturn;
    toReturn.density = densityOut;
    toReturn.radius = radiusOut;
    toReturn.diffusivity = diffusivityOut;
    return toReturn;
}


//// GEMS density vs distance from nuclei
//// GEMSdistanceFromCap(const Hypha &h)
//// segment 1018, 1004
//// VR??

//DTDoubleArray kernelRegresion(const DTDoubleArray &observations)
//{
//    int Nobs = observations.m()
//    double xa = Min observations
//}
