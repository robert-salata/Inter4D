#ifndef Models_class
#define Models_class

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include <d3dx11tex.h>
#include "material.h"
#include "shape3D.h"
#include "segment3D.h"
using namespace std;

class Models
{
public:
	struct ModelInfo
	{
		struct Face
		{
			Face(int index_start, int index_count, D3DXVECTOR3 center): index_start(index_start), index_count(index_count), center(center) { }
			int index_start;
			int index_count;
			D3DXVECTOR3 center;
		};
		vector<Face> faces;
		D3D_PRIMITIVE_TOPOLOGY topology;
		Material material;
		bool is_convex;
		int index_start;
		int index_count;
		D3DXVECTOR3 center;
	};

	Models(ID3D11Device* device);
	~Models();

	ModelInfo load(Shape3D* shape, Material material);
	ModelInfo load(Shape2D* shape, Material material);
	ModelInfo load(Segment3D* line, Material material);
	ModelInfo load_wireframe(Shape3D* shape, Material material);
	ModelInfo load_wireframe(Shape2D* shape, Material material);
	void reset();
	void render(ID3D11DeviceContext* context);
	void render(ID3D11DeviceContext* context, ModelInfo info);
	int get_index_count();

private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXCOLOR color;
		D3DXVECTOR3 normal;
	};

	static D3DXVECTOR3 to_vector(const Point3D* point);
	static D3DXVECTOR3 to_vector(const Point2D* point);
	void reload_buffers();

	ID3D11Device* device;
	ID3D11Buffer *vertex_buffer, *index_buffer;
	vector<VertexType> vertices;
	vector<int> indices;
};

#endif
