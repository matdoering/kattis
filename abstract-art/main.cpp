#include <vector>
#include <optional>
#include <iostream>
#include <utility>

using pdd = std::pair<int,int>;

struct Line {
    pdd A;
    pdd B;
};

struct Polygon {
    int nbrSides;
    std::vector<std::pair<int,int>> coords; // vertex xoordinates

    
    double surfaceArea() const {
        // algo explained here: https://www.mathopenref.com/coordpolygonarea2.html
        // assumption: positive y points upwards in coord system, polygon coords are in clockwise order
        int j = coords.size() - 1; // predecessor vertex
        double area = 0;
        for (int i = 0; i < coords.size(); ++i) {
            const auto& cur = coords[i];
            const auto& prev = coords[j];
            area += (prev.first + cur.first) * (prev.second - cur.second); // breadth * height of segment region
            j = i;
        }
        return area/2.0; 
    }
};

std::optional<std::pair<int, int>> lineIntersection(pdd& A, pdd& B, pdd& C, pdd& D) 
{ 
    // Line AB represented as a1x + b1y = c1 
    double a1 = B.second - A.second; 
    double b1 = A.first - B.first; 
    double c1 = a1*(A.first) + b1*(A.second); 
  
    // Line CD represented as a2x + b2y = c2 
    double a2 = D.second - C.second; 
    double b2 = C.first - D.first; 
    double c2 = a2*(C.first)+ b2*(C.second); 
  
    double determinant = a1*b2 - a2*b1; 
  
    if (determinant == 0) 
    { 
        // The lines are parallel. This is simplified 
        // by returning a pair of FLT_MAX 
        return std::nullopt;
    } 
    else
    { 
        double x = (b2*c1 - b1*c2)/determinant; 
        double y = (a1*c2 - a2*c1)/determinant; 
        return std::make_pair(x, y); 
    } 
}

std::optional<pdd> intersectOther(Line& line, std::vector<Polygon>& polys) {
    // this is complicated: 
    // a line segment may intersect with several other polygon line segments ...
    // how to determine the order of evaluation?
    return std::optional<pdd>();
}

/// Unifies polygons by forming their union (superpolygon)
Polygon unify(const std::vector<Polygon>& polys) {
   // start with the first polygon:
    const Polygon& p = polys[0];
    Line startLine;
    startLine.A = p.coords[0];
    startLine.B = p.coords[1];
    // algo:
    // intersect line with all other polygon lines
    // add the starting node and the intersection to the superpolygon
    // as well as the end point of the intersecting line
    // continue with the next segment in the polygon that had the intersecting line
    // ...
    // until we reach the start line again
    // finally: determine the area of the superpolygon
    // -> this is the canvas area covered
    return p;
}
int main(int argc, char** argv) {
    // parse input
    int nbr; // nbr of polygons to be pointed (by rows)
    std::cin >> nbr;
    std::vector<Polygon> polys; 
    while (nbr--) {
        // read polygon: {nbr sides} {x} {x} ...
        //std::cout << "no of poly: " << nbr << std::endl;
        Polygon p;
        int nbrSides;
        p.nbrSides = nbrSides;
        std::cin >> nbrSides;
        while (nbrSides--) {
            int x, y;
            std::cin >> x >> y;
            p.coords.push_back(std::make_pair(x,y));
        }
        polys.push_back(p);
    }
    double amountOfPaintUsed = 0;
    for (const Polygon& p : polys) {
        // amount of paint used: total surface area of all polygons
        amountOfPaintUsed += p.surfaceArea();
    }
    double usedCanvasArea = 0; 
    std::cout << amountOfPaintUsed << usedCanvasArea << std::endl;
}

