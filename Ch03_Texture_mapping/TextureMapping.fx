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
// TextureMapping
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string TextureMapping_Pass_0_Model : ModelData = ".\\sphere.x";

struct VS_INPUT
{
   float4 m_Position : POSITION;
   float2 m_TexCoord : TEXCOORD0;
};

struct VS_OUTPUT
{
   float4 m_Position : POSITION;
   float2 m_TexCoord : TEXCOORD0;
};

float4x4 g_worldMatrix : World;
float4x4 g_projectionMatrix : Projection;
float4x4 g_viewMatrix : View;

VS_OUTPUT TextureMapping_Pass_0_Vertex_Shader_vs_main(VS_INPUT Input)
{
   VS_OUTPUT Output;
   
   Output.m_Position = mul(Input.m_Position, g_worldMatrix);
   Output.m_Position = mul(Output.m_Position, g_viewMatrix);
   Output.m_Position = mul(Output.m_Position, g_projectionMatrix);
   
   Output.m_TexCoord = Input.m_TexCoord;
   
   return Output;
}
texture DiffuseMap_Tex
<
   string ResourceName = "..\\..\\..\\..\\..\\..\\Program Files (x86)\\AMD\\RenderMonkey 1.82\\Examples\\Media\\Textures\\EarthNight.jpg";
>;
sampler2D DiffuseSampler = sampler_state
{
   Texture = (DiffuseMap_Tex);
};
struct PS_INPUT
{
   float2 m_TexCoord : TEXCOORD0;
};

float4 TextureMapping_Pass_0_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR
{
   float4 albedo = tex2D(DiffuseSampler, Input.m_TexCoord);
   return albedo.rgba;
}
//--------------------------------------------------------------//
// Technique Section for TextureMapping
//--------------------------------------------------------------//
technique TextureMapping
{
   pass Pass_0
   {
      VertexShader = compile vs_2_0 TextureMapping_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 TextureMapping_Pass_0_Pixel_Shader_ps_main();
   }

}

