//
//  HyphaTools.cpp
//  meshSkel
//
//  Created by Jay Newby on 10/30/17.
//
//

#include "HyphaTools.h"


std::list<Hypha> collectHyphae(const DTDoubleArray &pointsWithSegments)
{
    std::list<Hypha> toReturn;
    int Npoints = pointsWithSegments.n();
    // compute Nsegs
    int Nsegs = 0;
    for (int n=0; n<Npoints; n++) {
        if (pointsWithSegments(3, n) > Nsegs) Nsegs = pointsWithSegments(3, n);
    }
    
    DTMutableIntArray NpointsPerSeg(Nsegs);
    // count number of points and tetras in each seg
    for (int s=1; s<=Nsegs; s++) {
        int nps = 0, nts = 0;
        for (int n=0; n<Npoints; n++) {
            if (pointsWithSegments(3, n) == s) nps++;
        }
        NpointsPerSeg(s-1) = nps;
    }
    for (int seg=1; seg<=Nsegs; seg++) {
        DTMutableDoubleArray segPoints(3, NpointsPerSeg(seg-1));
        for (int n=0, ns=0; n<Npoints; n++) {
            if (pointsWithSegments(3, n) == seg) {
                segPoints(0, ns) = pointsWithSegments(0, n);
                segPoints(1, ns) = pointsWithSegments(1, n);
                segPoints(2, ns) = pointsWithSegments(2, n);
                ns++;
            }
        }
        Hypha hout = remeshHypha(segPoints, seg);
        toReturn.push_back(hout);
    }
    return toReturn;
}

Hypha remeshHypha(const DTDoubleArray &segPoints, int seg)
{
    size_t Npoints = segPoints.n();
    std::list< std::pair<Point, unsigned> > listOfPoints;
    DTPointCollection3D points(segPoints);
    for (int n=0;n<Npoints;n++) {
        listOfPoints.push_front(std::make_pair(Point(segPoints(0, n),
                                                     segPoints(1, n),
                                                     segPoints(2, n)), n));
    }
    Delaunay T(listOfPoints.begin(), listOfPoints.end());
    
    Alpha_shape_3 as(T);
    // find optimal alpha values
    Alpha_shape_3::NT alpha_solid = as.find_alpha_solid();
    Alpha_iterator opt = as.find_optimal_alpha(1);
    int alphaOpt = *opt;
    int alpha = (alphaOpt > 300) ? alphaOpt + 50 : 300;
//    alpha *= 1.2;
    int Ncomp = 0, count = 0;
    while (true) {
        as.set_alpha(alpha);
        Ncomp = as.number_of_solid_components();
        if ((Ncomp == 1) || (count >= 10)) {
            //alpha *= 2.;
            break;
        }
        alpha += 50.;
        count++;
    }
    assert(Ncomp == 1);
    std::list<Cell_handle> cells;
    as.get_alpha_shape_cells(std::back_inserter(cells),
                             Alpha_shape_3::INTERIOR);
    size_t Ncells = cells.size();
    DTMutableIntArray offsets(4, Ncells);
//    Delaunay::Finite_cells_iterator cit = T.finite_cells_begin();
    int n=0;
    for (Cell_handle cit : cells) {
        for (int m=0; m<4; m++) {
            offsets(m, n) = cit->vertex(m)->info();
        }
        n++;
    }
    DTTetrahedralGrid3D tgrid(offsets, points);
    tgrid = AddNextTetrahedra(tgrid);
    Ashbya ash(tgrid);
    //ash = initialFilterByLength(ash, 60.);
    //ash = erosionFilterByLength(ash, 60.);
    ash = removeOrphanTetras(ash);
    //bool isvalid = ash.Tgrid().CheckIfNextTetraArrayIsValid();
    //Ashbya toReturn(ash.Tgrid(), ash.Volumes());
    //// add in segmentation data
    Hypha h = remeshSurface(ash, seg);
    return h;
    
}

Hypha remeshSurface(const Ashbya &ash, int seg)
{
    DTTetrahedralGrid3D tgrid = ash.Tgrid();
    DTSurface3D sgrid = ash.Sgrid();
    DTPointCollection3D surfPoints = tgrid.Points();
    size_t Npoints = surfPoints.NumberOfPoints();
    Triangle_mesh tmesh;
    DTIntArray pointSegments = ash.PointSegments();
    DTIntArray triangles = sgrid.Connections();
    size_t Ntri = triangles.n();
    DTIntArray connections = sgrid.Connections();
    //// find surface points (interior points are not filtered out of sgrid yet)
    DTMutableIntArray isSurf(Npoints);
    isSurf = 0;
    for (int n=0; n<Ntri; n++) {
        for (int m=0; m<3; m++) {
            isSurf(triangles(m, n)) = 1;
        }
    }
    ///// count the number of surface points
    //int NsurfPoints = 0;
    //for (int n=0; n<Npoints; n++) {NsurfPoints += isSurf(n);}
    //// add the surface points to the tmesh
    vertex_descriptor *surfVlist = (vertex_descriptor*) calloc(Npoints, sizeof(vertex_descriptor));
    Face_index *faceList = (Face_index*) calloc(Ntri, sizeof(Face_index));
    for (int n=0; n<Npoints; n++) {
        if (isSurf(n)!=1) continue;
        DTPoint3D p = surfPoints(n);
        surfVlist[n] = tmesh.add_vertex(Point3(p.x, p.y, p.z));
    }
    /////// add the triangles to the tmesh (points go ccw)
    for (int n=0; n<Ntri; n++) {
        int na = triangles(2, n),
        nb = triangles(1, n),
        nc = triangles(0, n);
        faceList[n] = tmesh.add_face(surfVlist[nc],
                                     surfVlist[nb],
                                     surfVlist[na]);
        //        if (tmesh.has_valid_index(faceList[n])==false) {
        //            int m=0;
        //        }
    }
    //    int Nfucked = 0;
    //    for (int n=0; n<Ntri; n++) {
    //        if (tmesh.has_valid_index(faceList[n])==false) {
    //            Nfucked++;
    //        }
    //    }
    free(surfVlist);
    free(faceList);
    //assert(CGAL::is_closed(tmesh));
    ///// remesh the surface to make it nice
    double target_edge_length = 7.0;
    unsigned int nb_iter = 3;
    PMP::isotropic_remeshing(faces(tmesh), target_edge_length, tmesh,
                             PMP::parameters::number_of_iterations(nb_iter).protect_constraints(false)
                             .fairing_continuity(1.5).number_of_relaxation_steps(2));
    tmesh.collect_garbage();
//    Triangle_mesh::Halfedge_range::iterator  hb, he;
//    for (auto hb = tmesh.halfedges_begin(); hb != tmesh.halfedges_end(); ++hb) {
//        std::vector<Face_index>  patch_facets;
//        std::vector<Vertex_handle> patch_vertices;
//        PMP::triangulate_refine_and_fair_hole(tmesh,
//                                              hb,
//                                              std::back_inserter(patch_facets),
//                                              std::back_inserter(patch_vertices),
//                                              PMP::parameters::vertex_point_map(get(CGAL::vertex_point, tmesh)).
//                                              geom_traits(Kernel()));
//
//    }
    
    int NsurfPoints = tmesh.num_vertices();
    DTMutableDoubleArray newSurfPointsData(3, NsurfPoints);
    
    //// this is probably unnecessary
    DTMutableIntArray vertexToPoint(NsurfPoints);
    
    
    Triangle_mesh::Vertex_range::iterator  vb, ve;
    //    Triangle_mesh::Vertex_range r = tmesh.vertices();
    // The iterators can be accessed through the C++ range API
    //    vb = r.begin();
    //    ve = r.end();
    //    for (boost::tie(vb, ve) = tmesh.vertices(); vb != ve; ++vb){
    int n=0;
    for (auto vb = tmesh.vertices_begin(); vb != tmesh.vertices_end(); ++vb) {
        Point3 surfp = tmesh.point(*vb);
        newSurfPointsData(0, n) = surfp.x();
        newSurfPointsData(1, n) = surfp.y();
        newSurfPointsData(2, n) = surfp.z();
        int vind = int(*vb);
        vertexToPoint(vind) = n;
        n++;
    }
    DTPointCollection3D newSurfPoints(newSurfPointsData);
    Ntri = tmesh.num_faces();
    DTMutableIntArray newTriangles(3, Ntri);
    n=0;
    for (auto fb = tmesh.faces_begin(); fb!=tmesh.faces_end(); ++fb) {
        CGAL::Vertex_around_face_iterator<Triangle_mesh> vb, ve;
        int m=0;
        for (boost::tie(vb, ve) = vertices_around_face(tmesh.halfedge(*fb), tmesh); vb!=ve; ++vb) {
            int vind = int(*vb);
            newTriangles(m, n) = vertexToPoint(vind);
            m++;
        }
        n++;
    }
    DTSurface3D newSurf(newTriangles, newSurfPoints);
    Hypha h;
    h.surface = newSurf;
    return h;
}
Hypha skeletonizeInit(const Hypha &H, int seg)
{
    DTSurface3D sgrid = H.surface;
    DTPointCollection3D surfPoints = sgrid.Points();
    size_t Npoints = surfPoints.NumberOfPoints();
    Triangle_mesh tmesh;
    DTIntArray triangles = sgrid.Connections();
    size_t Ntri = triangles.n();
    DTIntArray connections = sgrid.Connections();
    //// find surface points (interior points are not filtered out of sgrid yet)
    DTMutableIntArray isSurf(Npoints);
    isSurf = 0;
     for (int n=0; n<Ntri; n++) {
         for (int m=0; m<3; m++) {
             isSurf(triangles(m, n)) = 1;
         }
     }
    //// add the surface points to the tmesh
    vertex_descriptor *surfVlist = (vertex_descriptor*) calloc(Npoints, sizeof(vertex_descriptor));
    Face_index *faceList = (Face_index*) calloc(Ntri, sizeof(Face_index));
    for (int n=0; n<Npoints; n++) {
        if (isSurf(n)!=1) continue;
        DTPoint3D p = surfPoints(n);
        surfVlist[n] = tmesh.add_vertex(Point3(p.x, p.y, p.z));
    }
    /////// add the triangles to the tmesh (points go ccw)
    for (int n=0; n<Ntri; n++) {
        int na = triangles(2, n),
        nb = triangles(1, n),
        nc = triangles(0, n);
        faceList[n] = tmesh.add_face(surfVlist[nc],
                                     surfVlist[nb],
                                     surfVlist[na]);
    }
    free(surfVlist);
    free(faceList);
    int NsurfPoints = tmesh.num_vertices();
    //// this is probably unnecessary
    DTMutableIntArray vertexToPoint(NsurfPoints);
    
    
    Triangle_mesh::Vertex_range::iterator  vb, ve;
    int n=0;
    for (auto vb = tmesh.vertices_begin(); vb != tmesh.vertices_end(); ++vb) {
        int vind = int(*vb);
        vertexToPoint(vind) = n;
        n++;
    }
    Ntri = tmesh.num_faces();
    Hypha h;
    if (CGAL::is_valid(tmesh) &&  CGAL::is_closed(tmesh)) {
        h = makeSkeleton(sgrid, tmesh, vertexToPoint, seg);
    }
    else {h.surface = sgrid;}
    return h;
}
int getInd(int a, int b, int c)
{
    if (a == -1) return 0;
    else if (b == -1) return 1;
    else return 2;
}

Hypha makeSkeleton(const DTSurface3D &newSurf, const Triangle_mesh &tmesh,
                   const DTIntArray &vertexToPoint, int seg)
{
    int NsurfPoints = tmesh.num_vertices();
    ///// skeletonize the mesh
    Skeleton skeleton;
    //CGAL::extract_mean_curvature_flow_skeleton(tmesh, skeleton);
    Skeletonization mcs(tmesh);
    mcs.set_is_medially_centered(true);
    mcs.set_quality_speed_tradeoff(0.8);
    mcs.set_medially_centered_speed_tradeoff(0.3);
    mcs.contract_until_convergence();
    mcs.convert_to_skeleton(skeleton);
    //////////////////////////////////////////////////////////
    //// create mapping of surface points to skeleton points
    //// find number of surface points corresponding to each skeleton point
    DTMutableIntArray surfToSkel(NsurfPoints);
    surfToSkel = -1; // in case some surface points are not mapped to the skeleton
    int NskelPoints = num_vertices(skeleton);
    DTMutableDoubleArray skelPointData(3, NskelPoints);
    DTMutableIntArray skelToSurf(NsurfPoints);
    skelToSurf = -1;
    DTMutableIntArray skelToSurfIndex(NskelPoints, 2);
    skelToSurfIndex = -1;
    int n = 0;
    int count = 0;
    double dmax = 0.0;
    BOOST_FOREACH(Skeleton_vertex v, vertices(skeleton)) {
        Point3 skp = skeleton[v].point;
        skelPointData(0, n) = skp.x();
        skelPointData(1, n) = skp.y();
        skelPointData(2, n) = skp.z();
        DTPoint3D skPoint(skp.x(), skp.y(), skp.z());
        int sp = 0;
        BOOST_FOREACH(vertex_descriptor vd, skeleton[v].vertices) {
            Point3 surfp = get(CGAL::vertex_point, tmesh, vd);
            DTPoint3D surfPoint(surfp.x(), surfp.y(), surfp.z());
            double d = Distance(skPoint, surfPoint);
            if (d > dmax) dmax = d;
            surfToSkel(vertexToPoint(int(vd))) = n;
            skelToSurf(count) = vertexToPoint(int(vd));
            sp++;
            count++;
        }
        skelToSurfIndex(n, 1) = sp;
        if (sp > 0) skelToSurfIndex(n, 0) = count - sp;
        n++;
    }
    DTPointCollection3D skelPoints(skelPointData);
    //////////////////////////////////////////////////////////
    ///// extract skeleton graph structure
    DTMutableIntArray G(NskelPoints, 3);
    G = -1;
    BOOST_FOREACH(Skeleton_edge e, edges(skeleton))
    {
        int p0 = source(e, skeleton);
        int p1 = target(e, skeleton);
        int i0 = getInd(G(p0, 0), G(p0, 1), G(p0, 2));
        G(p0, i0) = p1;
        int i1 = getInd(G(p1, 0), G(p1, 1), G(p1, 2));
        G(p1, i1) = p0;
    }
    HyphaGraph hg = initHyphaGraph(G);
    int Nextra = hg.Ncaps - hg.Nbranches - 2;
    ///// find closest pair of caps to join if Nextra > 0
    for (int n=0; n<Nextra/2; n++) {
        double minDist = 1e20;
        int p0 = -1, p1 = -1;
        for (int i=0; i < hg.Ncaps; i++) {
            DTPoint3D x0 = skelPoints(hg.caps(i));
            for (int j=i+1; j < hg.Ncaps; j++) {
                DTPoint3D x1 = skelPoints(hg.caps(j));
                double d = Distance(x0, x1);
                if (d < minDist) {
                    minDist = d;
                    p0 = hg.caps(i);
                    p1 = hg.caps(j);
                }
            }
        }
        int i0 = getInd(G(p0, 0), G(p0, 1), G(p0, 2));
        G(p0, i0) = p1;
        int i1 = getInd(G(p1, 0), G(p1, 1), G(p1, 2));
        G(p1, i1) = p0;
        hg = initHyphaGraph(G);
    }
    alignSkel(hg.caps(0), 0, hg, G);
    hg = initHyphaGraph(G);
    /////// top level graph structure
    /// Nnodes = Nbranches + Ncaps + 2
    /// Ncaps = Nbranches + 2
    /// Nbranches = Nnodes/2 - 1
    /// Ncaps = Nnodes/2 + 1
    int Nnodes = 2*hg.Nbranches + 2;
    DTMutableIntArray branchGraph(Nnodes, 3);
    branchGraph = -1;
    DTMutableDoubleArray DS(hg.NskelPoints, 3);
    DS = -1.;
    DTMutableDoubleArray branchLengths(Nnodes, 3);
    branchLengths = -1.;
    if (Nnodes == 2) {
        branchGraph(0, 0) = 1;
        branchGraph(1, 1) = 0;
        double d = extractBranchArcLength(hg, skelPoints, DS, hg.caps(0), hg.NskelPoints-1, 0);
        branchLengths(0, 0) = d;
        branchLengths(1, 1) = d;
    }
    for (int n=0; n<hg.Nbranches; n++) {
        int b = hg.branches(n);
        for (int m=0; m<3; m++) {
            int c = travelBranch(hg, b, m);
            int N = NbranchElements(hg, b, m);
            double d = extractBranchArcLength(hg, skelPoints, DS, b, N, m);
            if (hg.z(c) == 3) {
                int cn = hg.branches.Find(c);
                branchGraph(n, m) = cn;
                branchLengths(n, m) = d;
            }
            else {
                int cn =  hg.Nbranches + hg.caps.Find(c);
                int reverse = (m == 2) ? 0:1;
                branchGraph(cn, reverse) = n;
                branchGraph(n, m) = cn;
                branchLengths(cn, reverse) = d;
                branchLengths(n, m) = d;
            }
        }
    }
    Hypha toReturn;
    toReturn.maxRadius = dmax;
    toReturn.surface = newSurf;
    toReturn.surfToSkeleton = surfToSkel;
    toReturn.skelToSurf = skelToSurf;
    toReturn.skelToSurfIndex = skelToSurfIndex;
    toReturn.skeleton = skelPoints;
    toReturn.DS = DS;
    toReturn.branchLengths = branchLengths;
    toReturn.hyphaGraph = hg;
    return toReturn;
}

int NbranchElements(const HyphaGraph &hg, int now, int direction)
{
    int upOrDown = (direction < 2) ? 0:1;
    int nxt = hg.skelGraph(now, direction);
    for (int n=0; n <= hg.NskelPoints; n++) {
        if (hg.z(nxt) != 2) return n + 1;
        now = nxt;
        nxt = hg.skelGraph(now, upOrDown);
    }
    return -1;
}

double extractBranchArcLength(const HyphaGraph &hg, const DTPointCollection3D &skelPoints,
                              DTMutableDoubleArray &DS,
                              int now, int N, int direction)
{
    int upOrDown = (direction < 2) ? 0:1;
    int reverse = (direction < 2) ? 1:0;
    int nxt = hg.skelGraph(now, direction);
    double totalDistance = 0.;
    for (int n=0; n < N; n++) {
        DTPoint3D x0 = skelPoints(now);
        DTPoint3D x1 = skelPoints(nxt);
        double d = Distance(x0, x1);
        if (hg.z(now) == 3) DS(now, direction) = d;
        else DS(now, upOrDown) = d;
        if (hg.z(nxt) < 3) DS(nxt, reverse) = d;
        totalDistance += d;
        now = nxt;
        nxt = hg.skelGraph(now, upOrDown);
    }
    return totalDistance;
}

int travelBranch(const HyphaGraph &hg, int now, int direction)
{
    int upOrDown = (direction < 2) ? 0:1;
    int nxt = hg.skelGraph(now, direction);
    for (int n=0; n <= hg.NskelPoints; n++) {
        if ((hg.z(nxt) == 1) || (hg.z(nxt) == 3)) return nxt;
        now = nxt;
        nxt = hg.skelGraph(now, upOrDown);
    }
    return -1;
}

int alignBranch(const HyphaGraph &hg, DTMutableIntArray &G, int now, int nxt)
{
    int past = now;
    for (int n=0; n <= 2*hg.NskelPoints; n++) {
        if (nxt == past) {
            int a = G(now, 0);
            int b = G(now, 1);
            G(now, 0) = b;
            G(now, 1) = a;
            nxt = G(now, 0);
            continue;
        }
        past = now;
        /////// if the point is a cap, we are done
        if (hg.z(nxt) == 1) {
            if (G(nxt, 0) != -1) {
                int b = G(nxt, 0);
                G(nxt, 0) = -1;
                G(nxt, 1) = b;
            }
            return nxt;
        }
        ////// if the point is a branch point align so that 0 and 1 point down stream and 2 upstream
        else if (hg.z(nxt) == 3) {
            if (G(nxt, 2) == past) {}
            else if (G(nxt, 0) == past) {
                int a = G(nxt, 0);
                int b = G(nxt, 2);
                G(nxt, 0) = b;
                G(nxt, 2) = a;
            }
            else {
                int a = G(nxt, 1);
                int b = G(nxt, 2);
                G(nxt, 1) = b;
                G(nxt, 2) = a;
            }
            return nxt;
        }
        now = nxt;
        nxt = G(now, 0);
    }
    return -1;
}

void alignSkel(int p0, int chan, const HyphaGraph &hg, DTMutableIntArray &G)
{
    int nxt = G(p0, chan);
    int p = alignBranch(hg, G, p0, nxt);
    for (int n=0; n<hg.Nbranches; n++) {
        if (p == hg.branches(n)) {
            alignSkel(p, 0, hg, G);
            alignSkel(p, 1, hg, G);
        }
    }
}

HyphaGraph initHyphaGraph(const DTIntArray &G)
{
    int NskelPoints = G.m();
    DTMutableIntArray z(NskelPoints);
    HyphaGraph toReturn;
    toReturn.skelGraph = G;
    int Ncaps = 0, Nbranches = 0;
    for (int n=0; n<NskelPoints; n++) {
        int zn = 0;
        for (int m=0; m<3; m++) {
            if (toReturn.skelGraph(n, m) > -1) zn++;
        }
        z(n) = zn;
        if (zn == 1) Ncaps++;
        if (zn == 3) Nbranches++;
    }
    DTMutableIntArray caps(Ncaps);
    DTMutableIntArray branches(Nbranches);
    int nc = 0, nb = 0;
    for (int n=0; n<NskelPoints; n++) {
        if (z(n) == 1) {
            caps(nc) = n;
            nc++;
        }
        else if (z(n) == 3) {
            branches(nb) = n;
            nb++;
        }
    }
    toReturn.caps = caps;
    toReturn.branches = branches;
    toReturn.Ncaps = Ncaps;
    toReturn.Nbranches = Nbranches;
    toReturn.NskelPoints = NskelPoints;
    toReturn.z = z;
    return toReturn;
}



