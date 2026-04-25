
#define PI 3.14159265359f
#define PI2 6.28318530718f  
//technique11 DefaultTechnique
//{
//    pass DefaultTechnique
//    {
//        SetDepthStencilState(Depth_Enable, 0);
//
//        VertexShader = compile vs_5_0 VS_MAIN();
//        PixelShader = compile ps_5_0 PS_MAIN();
//    }
//}

// D3D11_SAMPLER_DESC

	//D3D11_BLEND_DESC
	//D3D11_DEPTH_STENCIL_DESC
	//D3D11_RASTERIZER_DESC

	//m_pContext->RSSetState();
	//m_pContext->OMSetBlendState();
	// m_pContext->OMSetDepthStencilState();

RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState RS_WireFrame
{
    FillMode = WIREFRAME;
    CullMode = NONE;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_CW
{
    FillMode = Solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_Cull_None
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};


DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = All;
    DepthFunc = less_equal;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = Zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;


    SrcBlend[0] = Src_Alpha;
    DestBlend[0] = Inv_Src_Alpha;
    BlendOp[0] = Add;

    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = Inv_Src_Alpha;
    BlendOpAlpha[0] = Add;
    
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState BS_Blend
{
    BlendEnable[0] = true;

    SrcBlend[0] = One;
    DestBlend[0] = One;
    BlendOp[0] = Add;

    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = One;
    BlendOpAlpha[0] = Add;

    RenderTargetWriteMask[0] = 0x0F;
};