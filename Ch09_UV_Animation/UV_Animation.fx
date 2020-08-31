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
// UV_Animation
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string UV_Animation_Pass_0_torus : ModelData = ".\\torus.x";

struct VS_INPUT
{
   float4 m_Position : POSITION;
   float3 m_Normal : NORMAL;
   float2 m_UV     : TEXCOORD0;
};

struct VS_OUTPUT
{
   float4 m_Position      : POSITION;
   float2 m_UV            : TEXCOORD0;
   float3 m_Diffuse       : TEXCOORD1;
   float3 m_ViewDir       : TEXCOORD2;
   float3 m_Reflection    : TEXCOORD3;
};

float4x4 g_WorldMatrix : World;
float4x4 g_ProjectionMatrix : Projection;
float4x4 g_ViewMatrix : View;

float4 g_WorldLightPosition
<
   string UIName = "g_WorldLightPosition";
   string UIWidget = "Direction";
   bool UIVisible =  false;
   float4 UIMin = float4( -10.00, -10.00, -10.00, -10.00 );
   float4 UIMax = float4( 10.00, 10.00, 10.00, 10.00 );
   bool Normalize =  false;
> = float4( 500.00, 500.00, -500.00, 1.00 );
float4 g_WorldCameraPosition : ViewPosition;

float g_Time : Time0_X;
float g_WaveHeight
<
   string UIName = "g_WaveHeight";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 3.00 );
float g_Speed
<
   string UIName = "g_Speed";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 2.00 );
float g_WaveFrequency
<
   string UIName = "g_WaveFrequency";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 10.00 );
float g_UVSpeed
<
   string UIName = "g_UVSpeed";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float( 0.25 );

VS_OUTPUT UV_Animation_Pass_0_Vertex_Shader_vs_main(VS_INPUT Input) 
{
   VS_OUTPUT Output;
   
   float cosTime = g_WaveHeight * cos(g_Time * g_Speed + Input.m_UV.x * g_WaveFrequency);
   Input.m_Position.y += cosTime;
   
   Output.m_Position = mul(Input.m_Position, g_WorldMatrix);
   
   float3 lightDir = Output.m_Position.xyz - g_WorldLightPosition.xyz;
   lightDir = normalize(lightDir);
   
   float3 viewDir = normalize(Output.m_Position.xyz - g_WorldCameraPosition.xyz);
   Output.m_ViewDir = viewDir;
   
   Output.m_Position = mul(Output.m_Position, g_ViewMatrix);
   Output.m_Position = mul(Output.m_Position, g_ProjectionMatrix);
   
   float3 worldNormal = mul(Input.m_Normal, (float3x3)g_WorldMatrix);
   worldNormal = normalize(worldNormal);
   
   Output.m_Diffuse = dot(-lightDir, worldNormal);
   Output.m_Reflection = reflect(lightDir, worldNormal);
   
   Output.m_UV = Input.m_UV + float2(g_Time * g_UVSpeed, 0);
   
   return Output;
}
struct PS_INPUT
{
   float2 m_UV            : TEXCOORD0;
   float3 m_Diffuse       : TEXCOORD1;
   float3 m_ViewDir       : TEXCOORD2;
   float3 m_Reflection    : TEXCOORD3;
};

texture DiffuseMap_Tex
<
   string ResourceName = ".\\Fieldstone_DM.tga";
>;
sampler2D DiffuseSampler = sampler_state
{
   Texture = (DiffuseMap_Tex);
};
texture SpecularMap_Tex
<
   string ResourceName = ".\\fieldstone_SM.tga";
>;
sampler2D SpecularSampler = sampler_state
{
   Texture = (SpecularMap_Tex);
};

float3 g_LightColor
<
   string UIName = "g_LightColor";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float3( 0.70, 0.70, 1.00 );

float4 UV_Animation_Pass_0_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR
{
   float4 albedo = tex2D(DiffuseSampler, Input.m_UV);
   float3 diffuse =  g_LightColor * albedo.rgb * saturate(Input.m_Diffuse);
   
   float3 reflection = normalize(Input.m_Reflection);
   float3 viewDir = normalize(Input.m_ViewDir);
   float3 specular = 0;
   
   if(diffuse.x > 0)
   {
      specular = saturate(dot(reflection, -viewDir));
      specular = pow(specular, 20.0f);   
      
      float4 specularIntensity = tex2D(SpecularSampler, Input.m_UV);
      specular *= specularIntensity.rgb * g_LightColor;
   }
   
   float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo;
   
   return float4(ambient + diffuse + specular, 1);
}
//--------------------------------------------------------------//
// Technique Section for UV_Animation
//--------------------------------------------------------------//
technique UV_Animation
{
   pass Pass_0
   {
      VertexShader = compile vs_2_0 UV_Animation_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 UV_Animation_Pass_0_Pixel_Shader_ps_main();
   }

}

