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
// NormalMapping
//--------------------------------------------------------------//
//--------------------------------------------------------------//
// Pass 0
//--------------------------------------------------------------//
string NormalMapping_Pass_0_Sphere : ModelData = ".\\Teapot.x";

struct VS_INPUT
{
   float4 m_Position : POSITION;
   float3 m_Normal : NORMAL;
   float3 m_Tangent : TANGENT;
   float3 m_Binormal : BINORMAL;
   float2 m_UV     : TEXCOORD0;
};

struct VS_OUTPUT
{
   float4 m_Position      : POSITION;
   float2 m_UV            : TEXCOORD0;
   float3 m_LightDir      : TEXCOORD1;
   float3 m_ViewDir       : TEXCOORD2;
   float3 T               : TEXCOORD3;
   float3 B               : TEXCOORD4;
   float3 N               : TEXCOORD5;
};

float4x4 g_WorldViewProjectionMatrix : WorldViewProjection;
float4x4 g_WorldMatrix : World;

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

VS_OUTPUT NormalMapping_Pass_0_Vertex_Shader_vs_main(VS_INPUT Input) 
{
   VS_OUTPUT Output;
   
   Output.m_Position = mul(Input.m_Position, g_WorldViewProjectionMatrix);
   Output.m_UV = Input.m_UV;
   
   float4 worldPosition = mul(Input.m_Position, g_WorldMatrix);   
   float3 lightDir = worldPosition.xyz - g_WorldLightPosition.xyz;
   Output.m_LightDir = normalize(lightDir);
   
   float3 viewDir = normalize(Output.m_Position.xyz - g_WorldCameraPosition.xyz);
   Output.m_ViewDir = viewDir;
   
   float3 worldNormal = mul(Input.m_Normal, (float3x3)g_WorldMatrix);
   Output.N = normalize(worldNormal);
   
   float3 worldTangent = mul(Input.m_Tangent, (float3x3)g_WorldMatrix);
   Output.T = normalize(worldTangent);
   
   float3 worldBinormal = mul(Input.m_Binormal, (float3x3)g_WorldMatrix);
   Output.B = normalize(worldBinormal);   
   
   return Output;
}
struct PS_INPUT
{
   float2 m_UV            : TEXCOORD0;
   float3 m_LightDir      : TEXCOORD1;
   float3 m_ViewDir       : TEXCOORD2;
   float3 T               : TEXCOORD3;
   float3 B               : TEXCOORD4;
   float3 N               : TEXCOORD5;
};

texture DiffuseMap_Tex
<
   string ResourceName = "..\\..\\..\\..\\..\\..\\Program Files (x86)\\AMD\\RenderMonkey 1.82\\Examples\\Media\\Textures\\Fieldstone.tga";
>;
sampler2D DiffuseSampler = sampler_state
{
   Texture = (DiffuseMap_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
};
texture SpecularMap_Tex
<
   string ResourceName = ".\\fieldstone_SM.tga";
>;
sampler2D SpecularSampler = sampler_state
{
   Texture = (SpecularMap_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
};
sampler2D NormalSampler;
texture EnvironmentMap_Tex
<
   string ResourceName = "..\\..\\..\\..\\..\\..\\Program Files (x86)\\AMD\\RenderMonkey 1.82\\Examples\\Media\\Textures\\Snow.dds";
>;
samplerCUBE EnvironmentSampler = sampler_state
{
   Texture = (EnvironmentMap_Tex);
   MAGFILTER = LINEAR;
   MINFILTER = LINEAR;
};

float3 g_LightColor
<
   string UIName = "g_LightColor";
   string UIWidget = "Numeric";
   bool UIVisible =  false;
   float UIMin = -1.00;
   float UIMax = 1.00;
> = float3( 0.70, 0.70, 1.00 );

float4 NormalMapping_Pass_0_Pixel_Shader_ps_main(PS_INPUT Input) : COLOR
{
    float3 tangentNormal = tex2D(NormalSampler, Input.m_UV).xyz;
    // normal map RGB = normal vector XYZ * 0.5 + 0.5
    // normal vector XYZ = normal map RGB * 2 - 1
    tangentNormal = normalize(tangentNormal * 2 - 1); 
    tangentNormal = float3(0, 0, 1);
    
    float3x3 TBN = float3x3(normalize(Input.T), normalize(Input.B), normalize(Input.N));
    TBN = transpose(TBN);
    float3 worldNormal = mul(TBN, tangentNormal);
    
    float4 albedo = tex2D(DiffuseSampler, Input.m_UV);
    float3 lightDir = normalize(Input.m_LightDir);
    float3 diffuse = saturate(dot(worldNormal, -lightDir));
    diffuse = g_LightColor * albedo.rgb * diffuse;
    
    float3 specular = 0;
    
    float3 viewDir = normalize(Input.m_ViewDir);
    // if light hit the object
    if(diffuse.x > 0)
    {
       float3 reflection = reflect(lightDir, worldNormal);
       
       specular = saturate(dot(reflection, -viewDir));
       specular = pow(specular, 20.0f);
       
       float4 specularIntensity = tex2D(SpecularSampler, Input.m_UV);
       specular *= specularIntensity.rgb * g_LightColor;
    }
    
    float3 viewReflect = reflect(viewDir, worldNormal);
    float3 environment = texCUBE(EnvironmentSampler, viewReflect).rgb;
    
    float3 ambient = float3(0.1f, 0.1f, 0.1f) * albedo;
    
    return float4(ambient + diffuse + specular + environment, 1);
}
//--------------------------------------------------------------//
// Technique Section for NormalMapping
//--------------------------------------------------------------//
technique NormalMapping
{
   pass Pass_0
   {
      VertexShader = compile vs_2_0 NormalMapping_Pass_0_Vertex_Shader_vs_main();
      PixelShader = compile ps_2_0 NormalMapping_Pass_0_Pixel_Shader_ps_main();
   }

}

