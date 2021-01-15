Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
	float4 ambientColor;
    float3 lightDirection;
	float padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};

float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor = input.color;//shaderTexture.Sample(SampleType, input.tex);
	float lightIntensity = abs(dot(input.normal, -lightDirection));
    float4 color = (saturate(diffuseColor * lightIntensity) + ambientColor ) * textureColor;
	color.a = textureColor.a;
	return color;
}
