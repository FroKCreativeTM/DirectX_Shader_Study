//**************************************************************//
//  Effect File exported by RenderMonkey 1.6
//
//  - Although many improvements were made to RenderMonkey FX  
//    file export, there are still situations that may cause   
//    compilation problems once the file is exported, such as  
//    occasional naming conflicts for methods, since FX format 
//    does not support any notions of name spaces. You need to 
//    try to create workspaces in such a way as to minimize    
//    potential naming conflicts on export.                    
//    
//  - Note that to minimize resulting name collisions in the FX 
//    file, RenderMonkey will mangle names for passes, shaders  
//    and function names as necessary to reduce name conflicts. 
//**************************************************************//

//--------------------------------------------------------------//
// ShadowMapping
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// ApplyShadowTorus
//--------------------------------------------------------------//
string ShadowMapping_ApplyShadowTorus_Torus : ModelData = ".\\Torus.x";

struct VS_INPUT 
{
   float4 m_Position : POSITION0;
   float3 m_Normal : NORMAL;   
};

struct VS_OUTPUT 
{
   float4 m_Position : POSITION0;
   float4 m_ClipPosition : TEXCOORD1;
   float m_Diffuse : TEXCOORD2;
};

float4x4 g_WorldMatrix : World;
float4x4 g_LightViewMatrix
<
   string UIName = "g_LightViewMatrix";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
> = float4x4( 1.00, 0.00, 0.00, 0.00, 0.00, 1.00, 0.00, 0.00, 0.00, 0.00, 1.00, 0.00, 0.00, 0.00, 0.00, 1.00 );
float4x4 g_LightProjectionMatrix : Projection;

float4 g_WorldLightPosition
<
   string UIName = "g_WorldLightPosition";
   string UIWidget = "Direction";
   bool UIVisible =  false;
   float4 UIMin = float4( -10.00, -10.00, -10.00, -10.00 );
   float4 UIMax = float4( 10.00, 10.00, 10.00, 10.00 );
   bool Normalize =  false;
> = float4( 500.00, 500.00, -500.00, 1.00 );

float4x4 g_ViewProjectionMatrix : ViewProjection;

VS_OUTPUT ShadowMapping_ApplyShadowTorus_Vertex_Shader_vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;

   float4x4 lightViewMatrix = g_LightViewMatrix;
   
   float4 worldPosition = mul(Input.m_Position, g_WorldMatrix);
   Output.m_Position = mul(worldPosition, g_ViewProjectionMatrix);
   
   Output.m_ClipPosition = mul(worldPosition, lightViewMatrix);
   Output.m_ClipPosition = mul(Output.m_ClipPosition, g_LightProjectionMatrix);
   
   float3 lightDir = normalize(worldPosition.xyz - g_WorldLightPosition.xyz);
   float3 worldNormal = normalize(mul(Input.m_Normal, (float3x3)g_WorldMatrix));
   Output.m_Diffuse = dot(-lightDir, worldNormal);
   
   return( Output );
   
}




struct PS_INPUT
{
   float4 m_ClipPosition : TEXCOORD1;
   float m_Diffuse : TEXCOORD2;
};

texture ShadowMap_Tex
<
   string ResourceName = ".\\";
>;
sampler2D ShadowSampler = sampler_state
{
   Texture = (ShadowMap_Tex);
};
float4 g_ObjectColor
<
   string UIName = "g_ObjectColor";
   string UIWidget = "Color";
   bool UIVisible =  true;
> = float4( 1.00, 1.00, 0.00, 1.00 );

float4 ShadowMapping_ApplyShadowTorus_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR
{   
   float3 rgb = saturate(Input.m_Diffuse) * g_ObjectColor;
   
   float currentDepth = Input.m_ClipPosition.z / Input.m_ClipPosition.w;
 
   float2 uv = Input.m_ClipPosition.xy / Input.m_ClipPosition.w;
   uv.y = -uv.y;
   uv = uv * 0.5 + 0.5;
   
   float shadowDepth = tex2D(ShadowSampler, uv).r;
   
   if(currentDepth > shadowDepth + 0.0000125f)
   {
      rgb *= 0.5f;
   }  
   
   return (float4(rgb, 1.0f));
}
//--------------------------------------------------------------//
// Technique Section for ShadowMapping
//--------------------------------------------------------------//
technique ShadowMapping
{
   pass ApplyShadowTorus
   {
      VertexShader = compile vs_2_0 ShadowMapping_ApplyShadowTorus_Vertex_Shader_vs_main();
      PixelShader = compile ps_3_0 ShadowMapping_ApplyShadowTorus_Pixel_Shader_ps_main();
   }

}

