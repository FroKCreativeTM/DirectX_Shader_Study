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
// CreateShadow
//--------------------------------------------------------------//
string ShadowMapping_CreateShadow_Torus : ModelData = ".\\Torus.x";

texture ShadowMap_Tex : RenderColorTarget
<
   float2 RenderTargetDimensions = {512,512};
   string Format="D3DFMT_R32F";
   float  ClearDepth=1.000000;
   int    ClearColor=-1;
>;
float4x4 matViewProjection;

struct VS_INPUT 
{
   float4 m_Position : POSITION0;
   
};

struct VS_OUTPUT 
{
   float4 m_Position : POSITION0;
   float4 m_ClipPosition : TEXCOORD1;
};

float4x4 g_WorldMatrix : World;
float4x4 g_LightViewMatrix
<
   string UIName = "g_LightViewMatrix";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
> = float4x4( 1.00, 0.00, 0.00, 0.00, 0.00, 1.00, 0.00, 0.00, 0.00, 0.00, 1.00, 0.00, 0.00, 0.00, 0.00, 1.00 );
float4x4 g_LightProjectionMatrix : Projection;

float4 g_WorldLightPosition;

VS_OUTPUT ShadowMapping_CreateShadow_Vertex_Shader_vs_main( VS_INPUT Input )
{
   VS_OUTPUT Output;
   
   float4x4 lightViewMatrix = g_LightViewMatrix;
   
   Output.m_Position = mul(Input.m_Position, g_WorldMatrix);
   Output.m_Position = mul(Output.m_Position, lightViewMatrix);
   Output.m_Position = mul(Output.m_Position, g_LightProjectionMatrix);
   
   Output.m_ClipPosition = Output.m_Position;
   
   return( Output );
   
}




struct PS_INPUT
{
   float4 m_ClipPosition : TEXCOORD1;
};

float4x4 ShadowMapping_CreateShadow_Pixel_Shader_g_WorldMatrix;
float4x4 ShadowMapping_CreateShadow_Pixel_Shader_g_LightViewMatrix;
float4x4 ShadowMapping_CreateShadow_Pixel_Shader_g_LightProjectionMatrix;

float4 ShadowMapping_CreateShadow_Pixel_Shader_g_WorldLightPosition;

float4 ShadowMapping_CreateShadow_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR
{   
   // homogeneous coordinate 
   // w = 1 : Position
   // w = 0 : Direction
   float depth = Input.m_ClipPosition.z / Input.m_ClipPosition.w;
   // this is position vector
   return float4(depth.xxx, 1);
}
//--------------------------------------------------------------//
// Technique Section for ShadowMapping
//--------------------------------------------------------------//
technique ShadowMapping
{
   pass CreateShadow
   <
      string Script = "RenderColorTarget0 = ShadowMap_Tex;"
                      "ClearColor = (255, 255, 255, 255);"
                      "ClearDepth = 1.000000;";
   >
   {
      VertexShader = compile vs_2_0 ShadowMapping_CreateShadow_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 ShadowMapping_CreateShadow_Pixel_Shader_ps_main();
   }

}

