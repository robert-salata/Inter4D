#include "models.h"

#define SafeRelease(a) if(a){a->Release();a=NULL;}

Models::Models(ID3D11Device* device) 
{
	this->device = device;
	this->vertex_buffer = NULL;
	this->index_buffer = NULL;
}

Models::~Models()
{
	SafeRelease(vertex_buffer);
	SafeRelease(index_buffer);
}

D3DXVECTOR3 Models::to_vector(const Point3D* point)
{
	return D3DXVECTOR3(point->x.evaluate(), point->y.evaluate(), point->z.evaluate());
}

D3DXVECTOR3 Models::to_vector(const Point2D* point)
{
	return D3DXVECTOR3(point->x.evaluate(), point->y.evaluate(), 0);
}

Models::ModelInfo Models::load(Segment3D* line, Material material)
{
	Point3D point1 = Point3D(0, 0, 0);
	Point3D point2 = Point3D(0, 0, 0);
	line->get_ends(point1, point2);

	VertexType v1;
	v1.position = to_vector(&point1);
	v1.color = D3DXCOLOR(material.color.r, material.color.g, material.color.b, material.color.a);
	VertexType v2;
	v2.position = to_vector(&point2);
	v2.color = D3DXCOLOR(material.color.r, material.color.g, material.color.b, material.color.a);

	ModelInfo result = ModelInfo();
	result.faces.push_back(ModelInfo::Face(indices.size(), 2, (v1.position + v2.position) / 2));
	result.topology = D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
	result.material = material;
	result.index_start = indices.size();
	result.index_count = 2;
	result.center = (v1.position + v2.position) / 2;

	int vertices_count_so_far = this->vertices.size();
	vertices.push_back(v1);
	vertices.push_back(v2);
	indices.push_back(vertices_count_so_far);
	indices.push_back(vertices_count_so_far + 1);

	reload_buffers();

	return result;
}

Models::ModelInfo Models::load(Shape3D* shape, Material material)
{
	ModelInfo result = ModelInfo();
	result.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	result.material = material;
	result.index_start = indices.size();
	result.index_count = 0;
	result.center = to_vector(&shape->calculate_center());

	vector<Shape3D::Face*> shape_faces = shape->get_faces();

	for (auto it = shape_faces.begin(); it != shape_faces.end(); it++)
	{
		vector<Point3D*> face_vertices = shape->vertices_cycle_of_face(*it);
		if (face_vertices.size() < 3)
			continue;
		D3DXVECTOR3 face_normal = D3DXVECTOR3(0, 0, 0);

		int vertices_count_so_far = this->vertices.size();
		for (int i = 1; i < face_vertices.size() - 1; i++)
		{
			D3DXVECTOR3 v1 = to_vector(face_vertices[0]);
			D3DXVECTOR3 v2 = to_vector(face_vertices[i]);
			D3DXVECTOR3 v3 = to_vector(face_vertices[i + 1]);

			result.faces.push_back(ModelInfo::Face(indices.size(), 3, (v1+v2+v3)/3 ));
			result.index_count += 3;

			indices.push_back(vertices_count_so_far);
			indices.push_back(vertices_count_so_far + i);
			indices.push_back(vertices_count_so_far + i + 1);

			D3DXVECTOR3 edge1 = v2 - v1;
			D3DXVECTOR3 edge2 = v3 - v1;
			D3DXVECTOR3 triangle_normal;
			D3DXVec3Cross(&triangle_normal, &edge1, &edge2);
			face_normal += triangle_normal;
		}
		D3DXVec3Normalize(&face_normal, &face_normal);

		for (int i = 0; i < face_vertices.size(); i++)
		{
			VertexType v;
			v.position = D3DXVECTOR3(face_vertices[i]->x.evaluate(), face_vertices[i]->y.evaluate(), face_vertices[i]->z.evaluate());
			v.normal = face_normal;
			v.color = D3DXCOLOR(material.color.r, material.color.g, material.color.b, material.color.a);
			this->vertices.push_back(v);
		}
	}

	reload_buffers();

	return result;
}

Models::ModelInfo Models::load(Shape2D* shape, Material material)
{
	ModelInfo result = ModelInfo();
	result.topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	result.material = material;
	result.index_start = indices.size();
	result.index_count = 0;
	result.center = to_vector(&shape->calculate_center());

	vector<Point2D> face_vertices = shape->get_vertices_cycle();
	if (face_vertices.size() >= 3)
	{
		int vertices_count_so_far = this->vertices.size();
		for (int i = 1; i < face_vertices.size() - 1; i++)
		{
			D3DXVECTOR3 v1 = to_vector(&face_vertices[0]);
			D3DXVECTOR3 v2 = to_vector(&face_vertices[i]);
			D3DXVECTOR3 v3 = to_vector(&face_vertices[i + 1]);

			result.faces.push_back(ModelInfo::Face(indices.size(), 3, (v1+v2+v3)/3));
			result.index_count += 3;

			indices.push_back(vertices_count_so_far);
			indices.push_back(vertices_count_so_far + i);
			indices.push_back(vertices_count_so_far + i + 1);
		}

		for (int i = 0; i < face_vertices.size(); i++)
		{
			VertexType v;
			v.position = to_vector(&face_vertices[i]);
			v.color = D3DXCOLOR(material.color.r, material.color.g, material.color.b, material.color.a);
			this->vertices.push_back(v);
		}
	}

	reload_buffers();

	return result;
}

Models::ModelInfo Models::load_wireframe(Shape3D* shape, Material material)
{
	ModelInfo result = ModelInfo();
	result.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	result.material = material;
	result.index_start = indices.size();
	result.index_count = 0;
	result.center = to_vector(&shape->calculate_center());

	int vertices_count_so_far = this->vertices.size();
	vector<Point3D*> shape_vertices = shape->get_vertices();
	for (int i = 0; i < shape_vertices.size(); i++)
	{
		VertexType v;
		v.position = to_vector(shape_vertices[i]);
		v.color = D3DXCOLOR(material.color.r, material.color.g, material.color.b, material.color.a);
		this->vertices.push_back(v);
	}

	vector<Shape3D::Edge*> shape_edges = shape->get_edges();
	for (auto it = shape_edges.begin(); it != shape_edges.end(); it++)
	{
		result.faces.push_back(ModelInfo::Face(indices.size(), 2, (to_vector(shape_vertices[(*it)->first]) + to_vector(shape_vertices[(*it)->second]))/2));
		result.index_count += 2;

		indices.push_back(vertices_count_so_far + (*it)->first);
		indices.push_back(vertices_count_so_far + (*it)->second);
	}

	reload_buffers();

	return result;
}

Models::ModelInfo Models::load_wireframe(Shape2D* shape, Material material)
{
	ModelInfo result = ModelInfo();
	result.topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	result.material = material;
	result.index_start = indices.size();
	result.index_count = 0;
	result.center = to_vector(&shape->calculate_center());

	vector<Point2D> face_vertices = shape->get_vertices_cycle();

	int vertices_count_so_far = this->vertices.size();
	for (int i = 0; i < face_vertices.size() - 1; i++)
	{
		result.faces.push_back(ModelInfo::Face(indices.size(), 2, (to_vector(&face_vertices[i]) + to_vector(&face_vertices[i + 1])) / 2));
		result.index_count += 2;
		indices.push_back(vertices_count_so_far + i);
		indices.push_back(vertices_count_so_far + i + 1);
	}
	result.faces.push_back(ModelInfo::Face(indices.size(), 2, (to_vector(&face_vertices[0]) + to_vector(&face_vertices[face_vertices.size() - 1])) / 2));
	result.index_count += 2;
	indices.push_back(vertices_count_so_far + 0);
	indices.push_back(vertices_count_so_far + face_vertices.size() - 1);

	for (int i = 0; i < face_vertices.size(); i++)
	{
		VertexType v;
		v.position = D3DXVECTOR3(face_vertices[i].x.evaluate(), face_vertices[i].y.evaluate(), 0);
		v.color = D3DXCOLOR(material.color.r, material.color.g, material.color.b, material.color.a);
		this->vertices.push_back(v);
	}

	reload_buffers();

	return result;
}

void Models::reload_buffers()
{
	VertexType* vertices_buffer = new VertexType[this->vertices.size()];
	for (int i = 0; i < vertices.size(); i++)
		vertices_buffer[i] = vertices[i];

	unsigned long* indices_buffer = new unsigned long[this->indices.size()];
	for (int i = 0; i < indices.size(); i++)
		indices_buffer[i] = indices[i];

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices_buffer;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	SafeRelease(vertex_buffer);
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertex_buffer);

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices_buffer;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	SafeRelease(index_buffer);
	device->CreateBuffer(&indexBufferDesc, &indexData, &index_buffer);

	delete[] vertices_buffer;
	delete[] indices_buffer;
}

void Models::reset()
{
	SafeRelease(vertex_buffer);
	SafeRelease(index_buffer);
	vertices.clear();
	indices.clear();
}

void Models::render(ID3D11DeviceContext* context)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
}

void Models::render(ID3D11DeviceContext* context, ModelInfo info)
{
	context->IASetPrimitiveTopology(info.topology);
}


int Models::get_index_count()
{
	return indices.size();
}