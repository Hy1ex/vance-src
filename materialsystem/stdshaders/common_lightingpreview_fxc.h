#ifndef COMMON_LIGHTINGPREVIEW_FXC_H
#define COMMON_LIGHTINGPREVIEW_FXC_H

// DYNAMIC: "LIGHTING_PREVIEW" "0..1" [ = pShaderAPI->GetIntRenderingParameter( INT_RENDERPARM_ENABLE_FIXED_LIGHTING ) != 0 ]

// We don't want lighting preview yet.
// SKIP: $LIGHTING_PREVIEW

// Vertex Shader
#ifdef SHADER_MODEL_VS_3_0
#endif

// Pixel Shader
#ifndef SHADER_MODEL_VS_3_0
#endif

#endif // COMMON_LIGHTINGPREVIEW_FXC_H
