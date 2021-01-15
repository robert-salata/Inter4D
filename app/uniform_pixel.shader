Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

float4 UniformPixelShader(PixelInputType input) : SV_TARGET
{
	return input.color;
}
