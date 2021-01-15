#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

class DirectBase
{
public:
	DirectBase(int screen_width, int screen_height, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear, bool mode_3d = true);
	~DirectBase();
	
	void begin_scene(float, float, float, float);
	void end_scene();

	ID3D11Device* get_device();
	ID3D11DeviceContext* get_device_context();
	void get_projection_matrix(D3DXMATRIX&);
	void get_world_matrix(D3DXMATRIX&);
	void get_ortho_matrix(D3DXMATRIX&);
	void get_video_card_info(char*, int&);

	enum RasterizationType { BACK, FRONT, BOTH, FRAME };
	void set_rasterization(RasterizationType type);

private:
	bool vsync_enabled;
	int video_card_memory;
	char video_card_description[128];
	IDXGISwapChain* swap_chain;
	ID3D11Device* device;
	ID3D11DeviceContext* device_context;
	ID3D11RenderTargetView* render_target_view;
	ID3D11Texture2D* depth_stencil_buffer;
	ID3D11DepthStencilState* depth_stencil_state;
	ID3D11DepthStencilView* depth_stencil_view;
	ID3D11RasterizerState* front_rasterizer;
	ID3D11RasterizerState* back_rasterizer;
	ID3D11RasterizerState* both_rasterizer;
	ID3D11RasterizerState* frame_rasterizer;

	ID3D11BlendState* blend_state;
	D3DXMATRIX projection_matrix;
	D3DXMATRIX world_matrix;
	D3DXMATRIX ortho_matrix;
};

#endif