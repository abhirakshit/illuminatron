/**
* This code has been adapted from the code written by Marius Elvert
* available here: https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
*/
#pragma once

#include "CoreMinimal.h"
#include <unordered_map>
#include <vector>

 
struct Triangle
{
    int vert[3];
};

namespace icosahedron
{

    const float X = .525731112119133606f;
    const float Z = .850650808352039932f;
    const float N = 0.f;

    static const FVector vertices[] =
    {
        {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
        {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
        {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
    };

    static const Triangle triangles[] =
    {
        {0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
        {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
        {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
        {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
    };
}

struct uint32_pair_hash
{
    size_t operator()( const std::pair<uint32, uint32> &x ) const
    {
        union
        {
            struct
            {
                uint32 A;
                uint32 B;
            };
            size_t C;
        }jeffrey;
        jeffrey.A = x.first;
        jeffrey.B = x.second;
        return jeffrey.C;
    }
};

namespace std
{
    template<typename K, typename V>
    using umap = std::unordered_map<K, V, uint32_pair_hash>;
}

class icosphere
{
private:
	uint16 noise_seed = 1988;
    bool constructed = false;
    uint8 m_subdivisions = 0;
    TArray<FVector>  m_vertices;
    TArray<FVector2D> m_uvmapping;
    TArray<Triangle> m_triangles;
    TArray<int> m_indices;
    std::umap<std::pair<uint32, uint32>, uint32> lookup; //We keep this empty except while running
    void FindUV(const FVector &normal,FVector2D &uv);

protected:
    // normalizing should be redundant. todo: delete
    uint32 vertex_for_edge( uint32 vert_index_1, uint32 vert_index_2 );
    void subdivide();
    void mapuv();

public:
	icosphere();
	icosphere(icosphere &other);
    void normalize();
    void multiply(float radius);
    bool ready() const { return constructed; }
    uint8 size() const { return m_subdivisions; }
    void make_icosphere( uint8 subdivisions );
    const TArray<int>& get_indices() const { return m_indices; }
    const TArray<FVector>& get_vertices() const { return m_vertices; }
    const TArray<Triangle>& get_triangles() const { return m_triangles; }
    const TArray<FVector2D>& get_uvmapping() const { return m_uvmapping; }
    const FVector* get_vertices_raw() const { return m_vertices.GetData(); }
    const int* get_triangles_raw() const { return (int*)m_triangles.GetData(); }
    uint32 get_vert_count() const { return m_vertices.Num(); }
    uint32 get_tri_count() const { return m_triangles.Num(); }
    uint32 get_index_count() const { return m_indices.Num(); }
	void create_noise(float scale);

    void print();
    void print_vertices(int number);
};

