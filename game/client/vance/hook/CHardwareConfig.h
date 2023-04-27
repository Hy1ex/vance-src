#pragma once
#include "materialsystem/imaterialsystem.h"

#define MAT_DXLEVEL_OVERRIDE 100
#define MAT_DXLEVEL_OVERRIDE_S "100"

enum CompressedTextureState_t
{
	COMPRESSED_TEXTURES_ON,
	COMPRESSED_TEXTURES_OFF,
	COMPRESSED_TEXTURES_NOT_INITIALIZED
};

struct HardwareCaps_t : public MaterialAdapterInfo_t
{
	CompressedTextureState_t m_SupportsCompressedTextures;
	VertexCompressionType_t m_SupportsCompressedVertices;
	int  m_NumSamplers;
	int  m_NumTextureStages;
	int	 m_nMaxAnisotropy;
	int  m_MaxTextureWidth;
	int  m_MaxTextureHeight;
	int  m_MaxTextureDepth;
	int	 m_MaxTextureAspectRatio;
	int  m_MaxPrimitiveCount;
	int  m_NumPixelShaderConstants;
	int  m_NumBooleanPixelShaderConstants;
	int  m_NumIntegerPixelShaderConstants;
	int  m_NumVertexShaderConstants;
	int  m_NumBooleanVertexShaderConstants;
	int  m_NumIntegerVertexShaderConstants;
	int  m_TextureMemorySize;
	int  m_MaxNumLights;
	int  m_MaxBlendMatrices;
	int  m_MaxBlendMatrixIndices;
	int	 m_MaxVertexShaderBlendMatrices;
	int  m_MaxUserClipPlanes;
	HDRType_t m_HDRType;
	char m_pShaderDLL[32];
	ImageFormat m_ShadowDepthTextureFormat;
	ImageFormat m_NullTextureFormat;
	int  m_nVertexTextureCount;
	int  m_nMaxVertexTextureDimension;
	unsigned long m_AlphaToCoverageState;
	unsigned long m_AlphaToCoverageEnableValue;
	unsigned long m_AlphaToCoverageDisableValue;
	int	 m_nMaxViewports;
	float m_flMinGammaControlPoint;
	float m_flMaxGammaControlPoint;
	int m_nGammaControlPointCount;
	int m_MaxVertexShader30InstructionSlots;
	int m_MaxPixelShader30InstructionSlots;
	int m_MaxSimultaneousRenderTargets;
	bool m_bDeviceOk : 1;
	bool m_HasSetDeviceGammaRamp : 1;
	bool m_SupportsVertexShaders : 1;
	bool m_SupportsVertexShaders_2_0 : 1;
	bool m_SupportsPixelShaders : 1;
	bool m_SupportsPixelShaders_1_4 : 1;
	bool m_SupportsPixelShaders_2_0 : 1;
	bool m_SupportsPixelShaders_2_b : 1;
	bool m_SupportsShaderModel_3_0 : 1;
	bool m_bSupportsAnisotropicFiltering : 1;
	bool m_bSupportsMagAnisotropicFiltering : 1;
	bool m_bSupportsVertexTextures : 1;
	bool m_ZBiasAndSlopeScaledDepthBiasSupported : 1;
	bool m_SupportsMipmapping : 1;
	bool m_SupportsOverbright : 1;
	bool m_SupportsCubeMaps : 1;
	bool m_SupportsHardwareLighting : 1;
	bool m_SupportsMipmappedCubemaps : 1;
	bool m_SupportsNonPow2Textures : 1;
	bool m_PreferDynamicTextures : 1;
	bool m_HasProjectedBumpEnv : 1;
	bool m_SupportsSRGB : 1;
	bool m_bSupportsSpheremapping : 1;
	bool m_UseFastClipping : 1;
	bool m_bNeedsATICentroidHack : 1;
	bool m_bDisableShaderOptimizations : 1;
	bool m_bColorOnSecondStream : 1;
	bool m_bSupportsStreamOffset : 1;
	bool m_bFogColorSpecifiedInLinearSpace : 1;
	bool m_bFogColorAlwaysLinearSpace : 1;
	bool m_bSupportsAlphaToCoverage : 1;
	bool m_bSupportsShadowDepthTextures : 1;
	bool m_bSupportsFetch4 : 1;
	bool m_bSoftwareVertexProcessing : 1;
	bool m_bScissorSupported : 1;
	bool m_bSupportsFloat32RenderTargets : 1;
	bool m_bSupportsBorderColor : 1;
	bool m_bDX10Card : 1;
	bool m_bDX10Blending : 1;
	bool m_bSupportsStaticControlFlow : 1;
	bool m_FakeSRGBWrite : 1;
	bool m_CanDoSRGBReadFromRTs : 1;
	bool m_bSupportsGLMixedSizeTargets : 1;
	bool m_bCanStretchRectFromTextures : 1;
	HDRType_t m_MaxHDRType;
};

class IHardwareConfigInternal : public IMaterialSystemHardwareConfig
{
public:
	virtual const char *GetHWSpecificShaderDLLName() const = 0;
};

class CHardwareConfig : public IHardwareConfigInternal
{
public:

	static void OverrideHardwareCaps( CHardwareConfig *pHardwareConfig )
	{
		pHardwareConfig->m_ActualCaps.m_NumPixelShaderConstants = 224;	
		pHardwareConfig->m_Caps.m_NumPixelShaderConstants = 224;
		pHardwareConfig->m_UnOverriddenCaps.m_NumPixelShaderConstants = 224;

		pHardwareConfig->m_ActualCaps.m_nDXSupportLevel = MAT_DXLEVEL_OVERRIDE;	
		pHardwareConfig->m_Caps.m_nDXSupportLevel = MAT_DXLEVEL_OVERRIDE;
		pHardwareConfig->m_UnOverriddenCaps.m_nDXSupportLevel = MAT_DXLEVEL_OVERRIDE;

		pHardwareConfig->m_ActualCaps.m_nMaxDXSupportLevel = MAT_DXLEVEL_OVERRIDE;	
		pHardwareConfig->m_Caps.m_nMaxDXSupportLevel = MAT_DXLEVEL_OVERRIDE;
		pHardwareConfig->m_UnOverriddenCaps.m_nMaxDXSupportLevel = MAT_DXLEVEL_OVERRIDE;

		pHardwareConfig->m_ActualCaps.m_SupportsShaderModel_3_0 = true;	
		pHardwareConfig->m_Caps.m_SupportsShaderModel_3_0 = true;
		pHardwareConfig->m_UnOverriddenCaps.m_SupportsShaderModel_3_0 = true;
	}

	virtual bool HasDestAlphaBuffer() const;
	virtual bool HasStencilBuffer() const;
	virtual int	 GetFrameBufferColorDepth() const;
	virtual int  GetSamplerCount() const;
	virtual bool HasSetDeviceGammaRamp() const;
	virtual bool SupportsCompressedTextures() const;
	virtual VertexCompressionType_t SupportsCompressedVertices() const;
	virtual bool SupportsBorderColor() const;
	virtual bool SupportsFetch4() const;
	virtual bool CanStretchRectFromTextures() const;
	virtual bool SupportsVertexAndPixelShaders() const;
	virtual bool SupportsPixelShaders_1_4() const;
	virtual bool SupportsPixelShaders_2_0() const;
	virtual bool SupportsStaticControlFlow() const;
	virtual bool SupportsVertexShaders_2_0() const;
	virtual int  MaximumAnisotropicLevel() const;
	virtual int  MaxTextureWidth() const;
	virtual int  MaxTextureHeight() const;
	virtual int	 TextureMemorySize() const;
	virtual bool SupportsOverbright() const;
	virtual bool SupportsCubeMaps() const;
	virtual bool SupportsMipmappedCubemaps() const;
	virtual bool SupportsNonPow2Textures() const;
	virtual int  GetTextureStageCount() const;
	virtual int	 NumVertexShaderConstants() const;
	virtual int	 NumBooleanVertexShaderConstants() const;
	virtual int	 NumIntegerVertexShaderConstants() const;
	virtual int	 NumPixelShaderConstants() const;
	virtual int	 NumBooleanPixelShaderConstants() const;
	virtual int	 NumIntegerPixelShaderConstants() const;
	virtual int	 MaxNumLights() const;
	virtual bool SupportsHardwareLighting() const;
	virtual int	 MaxBlendMatrices() const;
	virtual int	 MaxBlendMatrixIndices() const;
	virtual int  MaxTextureAspectRatio() const;
	virtual int	 MaxVertexShaderBlendMatrices() const;
	virtual int	 MaxUserClipPlanes() const;
	virtual bool UseFastClipping() const;
	virtual int  GetDXSupportLevel() const;
	virtual const char *GetShaderDLLName() const;
	virtual bool ReadPixelsFromFrontBuffer() const;
	virtual bool PreferDynamicTextures() const;
	virtual bool SupportsHDR() const;
	virtual bool HasProjectedBumpEnv() const;
	virtual bool SupportsSpheremapping() const;
	virtual bool NeedsAAClamp() const;
	virtual bool NeedsATICentroidHack() const;
	virtual bool SupportsColorOnSecondStream() const;
	virtual bool SupportsStaticPlusDynamicLighting() const;
	virtual bool PreferReducedFillrate() const;
	virtual int	 GetMaxDXSupportLevel() const;
	virtual bool SpecifiesFogColorInLinearSpace() const;
	virtual bool SupportsSRGB() const;
	virtual bool FakeSRGBWrite() const;
	virtual bool CanDoSRGBReadFromRTs() const;
	virtual bool SupportsGLMixedSizeTargets() const;
	virtual bool IsAAEnabled() const;
	virtual int  GetVertexTextureCount() const;
	virtual int  GetMaxVertexTextureDimension() const;
	virtual int  MaxTextureDepth() const;
	virtual HDRType_t GetHDRType() const;
	virtual HDRType_t GetHardwareHDRType() const;
	virtual bool SupportsPixelShaders_2_b() const;
	virtual bool SupportsShaderModel_3_0() const;
	virtual bool SupportsStreamOffset() const;
	virtual int  StencilBufferBits() const;
	virtual int  MaxViewports() const;
	virtual void OverrideStreamOffsetSupport( bool bOverrideEnabled, bool bEnableSupport );
	virtual int  GetShadowFilterMode() const;
	virtual int NeedsShaderSRGBConversion() const;
	virtual bool UsesSRGBCorrectBlending() const;
	virtual bool HasFastVertexTextures() const;
	virtual int MaxHWMorphBatchCount() const;

	virtual bool ActuallySupportsPixelShaders_2_b() const;

	virtual bool SupportsHDRMode( HDRType_t nHDRMode ) const;

	virtual bool GetHDREnabled( void ) const;
	virtual void SetHDREnabled( bool bEnable );

protected:

	HardwareCaps_t m_ActualCaps;
	HardwareCaps_t m_Caps;
	HardwareCaps_t m_UnOverriddenCaps;
	bool m_bHDREnabled;
};

