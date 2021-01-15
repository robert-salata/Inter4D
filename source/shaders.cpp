#include "shaders.h"

void Shaders::render(ID3D11DeviceContext* deviceContext, int start_index, int index_count, Material material, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
	D3DXMATRIX projectionMatrix, D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor, D3DXVECTOR4 ambientColor)
{
	set_shader_parameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, NULL, lightDirection, diffuseColor, ambientColor);
	switch (material.type)
	{
	case NORMAL:
		render_shader(deviceContext, vertex_shader, normal_pixel_shader, start_index, index_count);
		break;
	case SELF_COLOR:
		render_shader(deviceContext, vertex_shader, uniform_pixel_shader, start_index, index_count);
		break;
	}
}

ID3D11VertexShader* Shaders::create_vertex_shader(WCHAR* filename, LPCSTR function_name)
{
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* vertexShaderBuffer = 0;
	HRESULT result = D3DX11CompileFromFile(filename, NULL, NULL, function_name, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		if (errorMessage)
			output_shader_error_message(errorMessage, hwnd, filename);
		else
			MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);
	}

	ID3D11VertexShader* result_shader;
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &result_shader);
	
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&layout);
	
	vertexShaderBuffer->Release();

	return result_shader;
}

ID3D11PixelShader* Shaders::create_pixel_shader(WCHAR* filename, LPCSTR function_name)
{
	ID3D10Blob* errorMessage = 0;
	ID3D10Blob* pixelShaderBuffer = 0;

	HRESULT result = D3DX11CompileFromFile(filename, NULL, NULL, function_name, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		if (errorMessage)
			output_shader_error_message(errorMessage, hwnd, filename);
		else
			MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);
	}

	ID3D11PixelShader* result_shader;
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &result_shader);
	pixelShaderBuffer->Release();
	return result_shader;
}

Shaders::Shaders(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{	
	this->hwnd = hwnd;
	this->device = device;
	vertex_shader = create_vertex_shader(L"vertex.shader", "LightVertexShader");
	normal_pixel_shader = create_pixel_shader(L"normal_pixel.shader", "LightPixelShader");
	uniform_pixel_shader = create_pixel_shader(L"uniform_pixel.shader", "UniformPixelShader");

	D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&samplerDesc, &sample_state);

	D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&matrixBufferDesc, NULL, &matrix_buffer);

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	device->CreateBuffer(&lightBufferDesc, NULL, &light_buffer);
}

Shaders::~Shaders()
{
	light_buffer->Release();
	matrix_buffer->Release();
	sample_state->Release();
	layout->Release();
	normal_pixel_shader->Release();
	uniform_pixel_shader->Release();
	vertex_shader->Release();
}

void Shaders::output_shader_error_message(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors = (char*)(errorMessage->GetBufferPointer());
	unsigned long bufferSize = errorMessage->GetBufferSize();

	ofstream fout;
	fout.open("shader-error.txt");
	for(int i=0; i<bufferSize; i++)
		fout << compileErrors[i];
	fout.close();

	errorMessage->Release();
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);
}

void Shaders::set_shader_parameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor, D3DXVECTOR4 ambientColor)
{
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	deviceContext->Map(matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	deviceContext->Unmap(matrix_buffer, 0);
    deviceContext->VSSetConstantBuffers(0, 1, &matrix_buffer);
	deviceContext->PSSetShaderResources(0, 1, &texture);

	deviceContext->Map(light_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	LightBufferType* dataPtr2 = (LightBufferType*)mappedResource.pData;
	dataPtr2->diffuse_color = diffuseColor;
	dataPtr2->ambient_color = ambientColor;
	dataPtr2->light_direction = lightDirection;
	dataPtr2->padding = 0.0f;
	deviceContext->Unmap(light_buffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &light_buffer);
}

void Shaders::render_shader(ID3D11DeviceContext* deviceContext, ID3D11VertexShader* vertex_shader, ID3D11PixelShader* pixel_shader, int start_index, int index_count)
{
	deviceContext->IASetInputLayout(layout);
    deviceContext->VSSetShader(vertex_shader, NULL, 0);
    deviceContext->PSSetShader(pixel_shader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &sample_state);
	deviceContext->DrawIndexed(index_count, start_index, 0);
}