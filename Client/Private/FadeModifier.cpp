#include "FadeModifier.h"
#include "UIRenderable.h"



CFadeModifier::CFadeModifier(EFade fadeType, _float duration, _bool useColor, _float4 color, _bool Inactive)
	: m_fadeType(fadeType),m_duration(duration), m_useColor(useColor), m_color(color), m_isInactive(Inactive), m_accTime(0.f)
{
	if (m_fadeType == FADE_IN)
		m_alpha = 0.f;
	else if (m_fadeType == FADE_OUT)
		m_alpha = 1.f;

}

CFadeModifier::~CFadeModifier()
{
	//if(m_This!=nullptr)
	//m_This->Set_Alpha(1);
}

void CFadeModifier::Tick(float fDeltaTime, CUI* pOwner)
{
	if(m_self == nullptr)
	{
		m_self = dynamic_cast<CUIRenderable*>(pOwner);



		if(m_useColor == true)
		{
			m_self->Set_UseColorMix(true);
			
		}

	
	}

	m_accTime += fDeltaTime;

	if (m_fadeType == FADE_OUT)
	{
	
	

		_float fRatio = m_accTime / m_duration;
		//LOG_F(LOG_LEVEL::INFO, "Time: %.4f / Duration: %.4f / Ratio: %.4f", m_AccTime, m_Duration, m_AccTime / m_Duration);

		if (fRatio > 1) fRatio = 1.f;

		m_alpha = 1 - fRatio;

		if (m_useColor == true)
		{
			m_self->Set_ColorMix(m_color);

		}

		m_self->Set_Alpha(m_alpha);

		if (m_alpha <= 0.01f)
		{
			if(m_isInactive ==true)
				pOwner->UI_InActive(); // UI 끄기

			m_isFinished = true;
		}



	}
		
	else if (m_fadeType == FADE_IN)
	{
		_float fRatio = m_accTime / m_duration;

		if (fRatio > 1) fRatio = 1.f;

		m_alpha = fRatio;

		if (m_useColor == true)
		{
			float lerpRadio = 1 - m_alpha;
			_float4 colorWAlpha = { m_color.x, m_color.y, m_color.z, lerpRadio };
			m_self->Set_ColorMix(m_color);

		}

		m_self->Set_Alpha(m_alpha);

		if (fRatio >= 1.f)
		{
			m_self->Set_Alpha(1.f);
			if(EndEvent != nullptr )
			{
				EndEvent();
			}
			//pOwner->UI_InActive(); // UI 끄기

			m_isFinished = true;
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
