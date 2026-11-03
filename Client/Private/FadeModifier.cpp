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

void CFadeModifier::Tick(float deltaTime, CUI* owner)
{
	if(m_self == nullptr)
	{
		m_self = dynamic_cast<CUIRenderable*>(owner);



		if(m_useColor == true)
		{
			m_self->Set_UseColorMix(true);
			
		}

	
	}

	m_accTime += deltaTime;

	if (m_fadeType == FADE_OUT)
	{
	
	

		_float ratio = m_accTime / m_duration;
		//LOG_F(LOG_LEVEL::INFO, "Time: %.4f / Duration: %.4f / Ratio: %.4f", m_AccTime, m_Duration, m_AccTime / m_Duration);

		if (ratio > 1) ratio = 1.f;

		m_alpha = 1 - ratio;

		if (m_useColor == true)
		{
			m_self->Set_ColorMix(m_color);

		}

		m_self->Set_Alpha(m_alpha);

		if (m_alpha <= 0.01f)
		{
			if(m_isInactive ==true)
				owner->UI_InActive(); // UI 끄기

			m_isFinished = true;
		}



	}
		
	else if (m_fadeType == FADE_IN)
	{
		_float ratio = m_accTime / m_duration;

		if (ratio > 1) ratio = 1.f;

		m_alpha = ratio;

		if (m_useColor == true)
		{
			float lerpRadio = 1 - m_alpha;
			_float4 colorWAlpha = { m_color.x, m_color.y, m_color.z, lerpRadio };
			m_self->Set_ColorMix(m_color);

		}

		m_self->Set_Alpha(m_alpha);

		if (ratio >= 1.f)
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

void CFadeModifier::OnAppear(float deltaTime, CUI* owner)
{
}

void CFadeModifier::OnDisappear(float deltaTime, CUI* owner)
{
	;
}
