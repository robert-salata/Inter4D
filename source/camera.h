#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <d3dx10math.h>

class Camera
{
public:
	Camera();

	void set_position(float, float, float);
	void set_rotation(float, float, float);

	void render();
	void get_view_matrix(D3DXMATRIX&);

	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;

private:
	D3DXMATRIX view_matrix;
};

#endif