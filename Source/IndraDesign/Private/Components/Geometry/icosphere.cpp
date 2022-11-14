#include "icosphere.h"
#include "icore.h"
#include "tudebug.h"
static uint64 logFlags = make_bitmask({ {4,4},{7,7} });/*comment these two lines to disable debug lines*/
//debugVeryVerbose(1, IndraGeometry, "Inverting normalized X");
//debugVeryVerbose(2, IndraGeometry, "Inverting normalized Z");
//debugVeryVerbose(3, IndraGeometry, "\nnorm.x = %.15f\nnorm.y = %.15f\nnorm.z = %.15f\nnormalizedX = %.15f\nnormalizedZ = %.15f\nU = %.15f\nV = %.15f", x, y, z, normalisedX, normalisedZ, uv.X, uv.Y);
//debugInfo(4, IndraGeometry, "Making icosphere with (%d) subdivisions.", subdivisions);
//debugInfo(5, IndraGeometry, "Subdividing icosphere\nInitial vertex count: %d", m_vertices.Num());
//debugInfo(6, IndraGeometry, "Subdividing icosphere\niteration: %d\ncurrent vertex count: %d\ncurrent triangle count: %d", i, m_vertices.Num(), m_triangles.Num());
//debugInfo(7, IndraGeometry, "Multiplying vertices by %f.", value);
//debugInfo(8, IndraGeometry, "subdivisions: %d", m_subdivisions);
//debugInfo(9, IndraGeometry, "vertices: %d", m_vertices.Num());
//debugInfo(10, IndraGeometry, "uv_mapping: %d", m_uvmapping.Num());
//debugInfo(11, IndraGeometry, "triangles: %d", m_triangles.Num());
//debugInfo(12, IndraGeometry, "indices: %d", m_indices.Num());
//debugInfo(13, IndraGeometry, "%s", *each.ToString());
//EndDebugBlock

#include <array>
#include <random>
static std::default_random_engine RNG;

uint8 spheres = 0;

icosphere::icosphere() {
    noise_seed += ++spheres;
}

icosphere::icosphere(icosphere & other) {
    noise_seed = other.noise_seed;
    constructed = other.constructed;
    m_subdivisions = other.m_subdivisions;
    m_vertices = other.m_vertices;
    m_uvmapping = other.m_uvmapping;
    m_triangles = other.m_triangles;
    m_indices = other.m_indices;
}

void icosphere::FindUV(const FVector & normal, FVector2D & uv) {
    const float& x = normal.X;
    const float& y = normal.Y;
    const float& z = normal.Z;
    float normalisedX = 0;
    float normalisedZ = -1;
    if (((x * x) + (z * z)) > 0) {
        normalisedX = FMath::Sqrt((x * x) / ((x * x) + (z * z)));
        if (x < 0) {
            debugVeryVerbose(1, IndraGeometry, "Inverting normalized X");
            normalisedX = -normalisedX;
        }
        normalisedZ = FMath::Sqrt((z * z) / ((x * x) + (z * z)));
        if (z < 0) {
            debugVeryVerbose(2, IndraGeometry, "Inverting normalized Z");
            normalisedZ = -normalisedZ;
        }
    }
    if (normalisedZ == 0) {
        uv.X = ((normalisedX * PI) / 2);
    } else {
        uv.X = FMath::Atan(normalisedX / normalisedZ);
        if (normalisedX < 0) {
            uv.X = PI - uv.X;
        }
        if (normalisedZ < 0) {
            uv.X += PI;
        }
    }
    if (uv.X < 0) {
        uv.X += 2 * PI;
    }
    uv.X /= 2 * PI;
    uv.Y = (-y + 1) / 2;
    debugVeryVerbose(3, IndraGeometry, "\nnorm.x = %.15f\nnorm.y = %.15f\nnorm.z = %.15f\nnormalizedX = %.15f\nnormalizedZ = %.15f\nU = %.15f\nV = %.15f", x, y, z, normalisedX, normalisedZ, uv.X, uv.Y);
}

void icosphere::make_icosphere(uint8 subdivisions) {
    LOGEVENT(Info);
    if (subdivisions > 10) {
        logWarning(IndraGeometry, "Cannot subdivide %d times, 10 is the limit. Clamping to 10.", subdivisions);
        subdivisions = 10;
    }
    debugInfo(4, IndraGeometry, "Making icosphere with (%d) subdivisions.", subdivisions);
    uint64 vertex_alloc = (1 << (2 * subdivisions)) * 10 + 2;
    m_vertices.Empty();
    m_vertices.Reserve(vertex_alloc);
    m_triangles.Empty();
    m_triangles.Reserve(20);

    m_vertices.Append(icosahedron::vertices, ARRAY_COUNT(icosahedron::vertices));
    m_triangles.Append(icosahedron::triangles, ARRAY_COUNT(icosahedron::triangles));
    normalize(); //just to be sure
    debugInfo(5, IndraGeometry, "Subdividing icosphere\nInitial vertex count: %d", m_vertices.Num());
    for (int i = 0; i < subdivisions; ++i) {
        subdivide();
        debugInfo(6, IndraGeometry, "Subdividing icosphere\niteration: %d\ncurrent vertex count: %d\ncurrent triangle count: %d", i, m_vertices.Num(), m_triangles.Num());
    }
    mapuv();
    constructed = true;
    m_subdivisions = subdivisions;
    m_indices = TArray<int>(get_triangles_raw(), m_triangles.Num() * 3);
}

void icosphere::normalize() {
    LOGEVENT(Info);
    for (FVector& each : m_vertices) {
        each.Normalize();
    }
}

void icosphere::multiply(float value) {
    debugInfo(7, IndraGeometry, "Multiplying vertices by %f.", value);
    for (FVector& each : m_vertices) {
        each *= value;
    }
}

uint32 icosphere::vertex_for_edge(uint32 vi1, uint32 vi2) {
    uint32 a = std::min(vi1, vi2);
    uint32 b = std::max(vi1, vi2);
    std::pair<uint32, uint32> key(a, b);

    auto inserted = lookup.insert({ key, m_vertices.Num() });
    if (inserted.second) {
        FVector& edge0 = m_vertices[a];
        FVector& edge1 = m_vertices[b];
        auto point = edge0 + edge1;
        point.Normalize();
        m_vertices.Add(point);
    }

    return inserted.first->second;
}

//todo: add log lines; todo-r2: is this todo necessary?
void icosphere::subdivide() {
    TArray<Triangle> swap_sphere;
    swap_sphere.Reserve(m_triangles.Num() * 4);

    for (auto&& triangle : m_triangles) {
        std::array<int, 3> mid;
        for (int edge = 0; edge < 3; ++edge) {
            mid[edge] = vertex_for_edge(triangle.vert[edge], triangle.vert[(edge + 1) % 3]);
        }

        swap_sphere.Add({ triangle.vert[0], mid[0], mid[2] });
        swap_sphere.Add({ triangle.vert[1], mid[1], mid[0] });
        swap_sphere.Add({ triangle.vert[2], mid[2], mid[1] });
        swap_sphere.Add({ mid[0], mid[1], mid[2] });
    }
    Swap(m_triangles, swap_sphere); // no new memory needed
    lookup.clear();
}

void icosphere::mapuv() {
    LOGEVENT(Info);
    m_uvmapping.Empty();
    m_uvmapping.Reserve(m_vertices.Num());
    for (int32 i = 0; i < m_vertices.Num(); ++i) {
        FVector2D uv;
        FindUV(m_vertices[i], uv);
        m_uvmapping.Add(uv);
    }
}

void icosphere::create_noise(float alpha) {
    //scale = FMath::IsNearlyZero(scale) ? 1 : scale;
    static std::uniform_real_distribution<float> dist(1, 1.5);
    RNG.seed(noise_seed);
    for (FVector& each : m_vertices) {
        each = FMath::Lerp(each, each * dist(RNG), alpha);
        /*each.X += dist(RNG) * scale;
        each.Y += dist(RNG) * scale;
        each.Z += dist(RNG) * scale;/**/
    }
}

void icosphere::print() {
    debugInfo(8, IndraGeometry, "subdivisions: %d", m_subdivisions);
    debugInfo(9, IndraGeometry, "vertices: %d", m_vertices.Num());
    debugInfo(10, IndraGeometry, "uv_mapping: %d", m_uvmapping.Num());
    debugInfo(11, IndraGeometry, "triangles: %d", m_triangles.Num());
    debugInfo(12, IndraGeometry, "indices: %d", m_indices.Num());
}

void icosphere::print_vertices(int number) {
    int count = 0;
    for (FVector& each : m_vertices) {
        debugInfo(13, IndraGeometry, "%s", *each.ToString());
        if (++count == number) {
            return;
        }
    }
}
