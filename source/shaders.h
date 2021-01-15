#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include "material.h"
using namespace std;

class Shaders
{
public:
	Shaders(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	Shaders::~Shaders();

	void render(ID3D11DeviceContext* deviceContext, int start_index, int index_count, Material material, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
		D3DXMATRIX projectionMatrix, D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor, D3DXVECTOR4 ambientColor);

private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 diffuse_color;
		D3DXVECTOR4 ambient_color;
		D3DXVECTOR3 light_direction;
		float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};

	ID3D11VertexShader* create_vertex_shader(WCHAR* filename, LPCSTR function_name);
	ID3D11PixelShader* create_pixel_shader(WCHAR* filename, LPCSTR function_name);
	void output_shader_error_message(ID3D10Blob*, HWND, WCHAR*);
	void set_shader_parameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);
	void render_shader(ID3D11DeviceContext*, ID3D11VertexShader* vertex_shader, ID3D11PixelShader* pixel_shader, int start_index, int index_count);

	HWND hwnd;
	ID3D11Device* device;
	ID3D11VertexShader* vertex_shader;
	ID3D11PixelShader* normal_pixel_shader;
	ID3D11PixelShader* uniform_pixel_shader;
	ID3D11InputLayout* layout;
	ID3D11SamplerState* sample_state;
	ID3D11Buffer* matrix_buffer;
	ID3D11Buffer* light_buffer;
};

#endif