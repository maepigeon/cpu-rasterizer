

#include "IOEngine/InputGeometryQueries.hpp"

InputGeometryQueries::InputGeometryQueries(Rasterizer* rasterizer, float scale) {
    this->rasterizer = rasterizer;
    this->scale = scale;
}

PickResult InputGeometryQueries::pickAtWindowPos(float wx, float wy) {
    int x = (int)(wx / scale); int y = (int)(wy / scale);
    if (x < 0 || y < 0 || x >= rasterizer->width || y >= rasterizer->height) return {};
    GeometryReference& geoRef = rasterizer->getGeometryBuffer()[y * rasterizer->width + x];
    if ( geoRef.triangleIndex < 0 ) return {};
    TriSource src = rasterizer->getTriSource(geoRef.triangleIndex);
    return { true, src.meshIndex, src.triIndex, geoRef.alpha, geoRef.beta, geoRef.gama };
}