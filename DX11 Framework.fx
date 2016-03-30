

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 DiffuseMtrl; // The same as XMFLOAT4 but cannot use it here.
	float4 DiffuseLight;
	float3 LightVecW;
	float gTime;
	float4 AmbientMaterial;
	float4 AmbientLight;
	float4 SpecularMtrl;
	float4 SpecularLight;
	float SpecularPower;
	float3 EyePosW;



}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Norm : NORMAL;
	float3 PosW : POSITION;
};


//------------------------------------------------------------------------------------
// Vertex Shader - Implements Gouraud Shading using Diffuse lighting only
//------------------------------------------------------------------------------------
VS_OUTPUT VS(float4 Pos : POSITION, float3 NormalL : NORMAL, float2 Tex : TEXCOORD)
{

	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(Pos, World);
	output.PosW = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	float3 normalW = mul(float4(NormalL, 0.0f), World).xyz; // more eff
	normalW = normalize(normalW);

	output.Norm = normalW;

	output.Tex = Tex;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
	float3 toEye = normalize(EyePosW - input.PosW.xyz);

	// Convert from local space to world space 
	// W component of vector is 0 as vectors cannot be translated
	
	float3 normalW = normalize(input.Norm);

	float3 r = reflect(-LightVecW, normalW);

	float specularAmount = pow(max(dot(r, toEye), 0.0f), SpecularPower);

	// Compute Colour using Diffuse lighting only
	float diffuseAmount = max(dot(LightVecW, normalW), 0.0f);

	float4 Color;

	float3 diffuse = diffuseAmount * (DiffuseMtrl * DiffuseLight).rgb;
		
	// Ambient 
	float3 ambient = AmbientMaterial * AmbientLight;

	float3 specular = specularAmount * (SpecularMtrl * SpecularLight).rgb;

	Color.rgb = ambient + diffuse + specular;
	
	Color.a = DiffuseMtrl.a;
		
	float4 textureColour = txDiffuse.Sample(samLinear, input.Tex);
	
	return textureColour + Color;
	
}
