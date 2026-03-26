#include "FadeModifier.h"
#include "UIRenderable.h"



CFadeModifier::CFadeModifier(FADE fadeType, _float duration, _bool useColor, _float4 color)
	: m_fadeType(fadeType),m_Duration(duration), m_useColor(useColor), m_color(color)
{
	if (m_fadeType == FADE_IN)
		m_Alpha = 0.f;
	else if (m_fadeType == FADE_OUT)
		m_Alpha = 1.f;

}

CFadeModifier::~CFadeModifier()
{
	if(m_This!=nullptr)
	m_This->Set_Alpha(1);
}

void CFadeModifier::Tick(float fDeltaTime, CUI* pOwner)
{
	if(m_This == nullptr)
	{
		m_This = dynamic_cast<CUIRenderable*>(pOwner);



		if(m_useColor == true)
		{
			m_This->Set_UseColorMix(true);
			
		}

	
	}

	m_AccTime += fDeltaTime;

	if (m_fadeType == FADE_OUT)
	{

		_float fRatio = m_AccTime / m_Duration;

		if (fRatio > 1) fRatio = 1.f;

		m_Alpha = 1 - fRatio;

		if (m_useColor == true)
		{
			m_This->Set_ColorMix(m_color);

		}

		m_This->Set_Alpha(m_Alpha);

		if (m_Alpha <= 0.f)
		{
			pOwner->UI_InActive(); // UI ²ô±â

			m_Finished = true;
		}

	}
		
	else if (m_fadeType == FADE_IN)
	{
		_float fRatio = m_AccTime / m_Duration;

		if (fRatio > 1) fRatio = 1.f;

		m_Alpha = fRatio;

		if (m_useColor == true)
		{
			float lerpRadio = 1 - m_Alpha;
			_float4 colorWAlpha = { m_color.x, m_color.y, m_color.z, lerpRadio };
			m_This->Set_ColorMix(m_color);

		}

		m_This->Set_Alpha(m_Alpha);

		if (m_Alpha <= 0.f)
		{
			pOwner->UI_InActive(); // UI ²ô±â

			m_Finished = true;
		}
	}
		

}

void CFadeModifier::OnAppear(float fDeltaTime, CUI* pOwner)
{
}

void CFadeModifier::OnDisappear(float fDeltaTime, CUI* pOwner)
{
	;
}
