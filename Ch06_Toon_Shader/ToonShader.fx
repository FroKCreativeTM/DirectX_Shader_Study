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
// ToonShader
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string ToonShader_Pass_0_Teapot : ModelData = "..\\..\\..\\..\\..\\..\\Program Files (x86)\\AMD\\RenderMonkey 1.82\\Examples\\Media\\Models\\Teapot.3ds";

struct VS_INPUT 
{
   float4 m_Position : POSITION;
   float3 m_Normal : NORMAL;
};

struct VS_OUTPUT 
{
   float4 m_Position : POSITION;
   float3 m_Diffuse : TEXCOORD1;
};

float4x4 g_WorldViewProjectionMatrix : WorldViewProjection;
float4x4 g_InverseWorldMatrix : WorldInverse;

float4 g_WorldLightPosition
<
   string UIName = "g_WorldLightPosition";
   string UIWidget = "Direction";
   bool UIVisible =  false;
   float4 UIMin = float4( -10.00, -10.00, -10.00, -10.00 );
   float4 UIMax = float4( 10.00, 10.00, 10.00, 10.00 );
   bool Normalize =  false;
> = float4( 500.00, 500.00, -500.00, 1.00 );

VS_OUTPUT ToonShader_Pass_0_Vertex_Shader_vs_main(VS_INPUT Input)
{
   VS_OUTPUT Output;

   Output.m_Position = mul(Input.m_Position, g_WorldViewProjectionMatrix);
   
   float3 objectLightPosition = mul(g_WorldLightPosition , g_InverseWorldMatrix);
   float3 lightDir = normalize(Input.m_Position.xyz - objectLightPosition);
   
   Output.m_Diffuse = dot(-lightDir, normalize(Input.m_Normal));
   
   return Output;
}
float3 g_SurfaceColor
<
   string UIName = "g_SurfaceColor";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float3( 0.34, 0.84, 0.66 );

struct PS_INPUT
{
   float3 m_Diffuse : TEXCOORD1;
};

float4 ToonShader_Pass_0_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR
{   
   float3 diffuse = saturate(Input.m_Diffuse);
   diffuse = ceil(diffuse * 5) / 5.0f;
   return float4(g_SurfaceColor * diffuse.xyz, 1);
}
//--------------------------------------------------------------//
// Technique Section for ToonShader
//--------------------------------------------------------------//
technique ToonShader
{
   pass Pass_0
   {
      VertexShader = compile vs_2_0 ToonShader_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 ToonShader_Pass_0_Pixel_Shader_ps_main();
   }

}

