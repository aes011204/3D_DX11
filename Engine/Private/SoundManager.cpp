
#include "SoundManager.h"

//#include <fmod_errors.h>

using namespace std;
using namespace DirectX;
using namespace SimpleMath;
using namespace Engine;




namespace
{


	[[nodiscard]] inline _string WstrToStr_UTF8(const _wstring& wstrText)
	{
		const int iRequiredSize = WideCharToMultiByte(
			CP_UTF8,
			0,
			wstrText.data(),
			static_cast<int>(wstrText.size()),
			nullptr,
			0,
			nullptr,
			nullptr
		);

		if (iRequiredSize <= 0)
			return {};

		_string strText;
		strText.resize(static_cast<size_t>(iRequiredSize));

		const int iConvertedSize = WideCharToMultiByte(
			CP_UTF8,
			0,
			wstrText.data(),
			static_cast<int>(wstrText.size()),
			strText.data(),
			iRequiredSize,
			nullptr,
			nullptr
		);

		if (iConvertedSize <= 0)
			return {};

		return strText;
	}

	
	[[nodiscard]] inline _string CharToStr(const _char* pText)
	{
		return nullptr != pText ? _string{ pText } : _string{};
	}

	
	[[nodiscard]] inline _wstring TCharToWstr(const _tchar* pText)
	{
		return nullptr != pText ? _wstring{ pText } : _wstring{};
	}

	constexpr size_t s_uiSoundCategoryCount = static_cast<size_t>(SOUND_CATEGORY::END);

	constexpr array<const char*, s_uiSoundCategoryCount> s_CategoryGroupNames =
	{
		"BGM",
		"SFX",
		"UI",
		"VOICE",
		"AMBIENT",
	};

	void Log_FMODFailure(
		const FMOD_RESULT eResult,
		const _char* pOperation,
		const _wstring& strSoundTag = {})
	{
		//const _char* pErrorText = FMOD_ErrorString(eResult);
		if (strSoundTag.empty())
		{
			//LOG_ERROR(
			//    "FMOD call failed. Operation='{}' Result={} Error='{}'",
			//    nullptr != pOperation ? pOperation : "<null>",
			//    static_cast<_int>(eResult),
			//    nullptr != pErrorText ? pErrorText : "<null>"
			//);
			return;
		}

		//LOG_ERROR(
		//    "FMOD call failed. Operation='{}' SoundTag='{}' Result={} Error='{}'",
		//    nullptr != pOperation ? pOperation : "<null>",
		//    String::WstrToStr_UTF8(strSoundTag),
		//    static_cast<_int>(eResult),
		//    nullptr != pErrorText ? pErrorText : "<null>"
		//);
	}

	_float ClampVolume(const _float fVolume)
	{
		return clamp(fVolume, 0.f, 1.f);
	}
}

CSoundManager::CSoundManager()
{
}

CSoundManager::~CSoundManager()
{
	Free();
}

HRESULT CSoundManager::Initialize()
{
	FMOD_RESULT eResult = FMOD::System_Create(&m_pSystem);
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "System_Create");
		return E_FAIL;
	}

	eResult = m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr);
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "System::init");
		return E_FAIL;
	}

	for (size_t uiIndex = 0; uiIndex < s_uiSoundCategoryCount; ++uiIndex)
	{
		eResult = m_pSystem->createChannelGroup(
			s_CategoryGroupNames[uiIndex],
			&m_pCategoryGroups[uiIndex]
		);
		if (FMOD_OK != eResult)
		{
			Log_FMODFailure(eResult, "System::createChannelGroup");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSoundManager::Register_Sounds(const vector<SOUND_ASSET_DESC>& vecSoundDescs)
{
	if(nullptr==m_pSystem)return E_FAIL;

	for (const SOUND_ASSET_DESC& tSoundDesc : vecSoundDescs)
	{
		if (tSoundDesc.strSoundTag.empty())
		{
			//LOG_ERROR("Sound registration failed. soundTag must not be empty.");
			return E_FAIL;
		}

		if (tSoundDesc.strAssetPath.empty())
		{
			//LOG_ERROR(
			//    "Sound registration failed. assetPath must not be empty. soundTag='{}'",
			//    String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
			//);
			return E_FAIL;
		}

		if (!Is_ValidCategory(tSoundDesc.eCategory))
		{
			//LOG_ERROR(
			//    "Sound registration failed. Invalid category. soundTag='{}'",
			//    String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
			//);
			return E_FAIL;
		}

		if (!Is_ValidLoadMode(tSoundDesc.eLoadMode))
		{
			// LOG_ERROR(
			//     "Sound registration failed. Invalid loadMode. soundTag='{}'",
			//     String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
			// );
			return E_FAIL;
		}

		if (0.f > tSoundDesc.fDefaultVolume || 1.f < tSoundDesc.fDefaultVolume)
		{
			//LOG_ERROR(
			//    "Sound registration failed. defaultVolume must be in [0, 1]. soundTag='{}' Volume={}",
			//    String::WstrToStr_UTF8(tSoundDesc.strSoundTag),
			//    tSoundDesc.fDefaultVolume
			//);
			return E_FAIL;
		}

		if (m_SoundDescs.contains(tSoundDesc.strSoundTag))
		{
			//LOG_ERROR(
			//    "Sound registration failed. Duplicate soundTag='{}'",
			//    String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
			//);
			return E_FAIL;
		}

		if (FAILED(Create_Sound(tSoundDesc)))
			return E_FAIL;

		m_SoundDescs.emplace(tSoundDesc.strSoundTag, tSoundDesc);
	}

	return S_OK;
}

void CSoundManager::Update_Audio(const _float fTimeDelta)
{
	if (nullptr == m_pSystem)
		return;

	const FMOD_RESULT eResult = m_pSystem->update();
	if (FMOD_OK != eResult)
		Log_FMODFailure(eResult, "System::update");

	Cleanup_FinishedLoopChannels();
	Cleanup_FinishedFadeChannels();

	auto iter = m_FadingLoopChannels.begin();
	while (iter != m_FadingLoopChannels.end())
	{
		if (nullptr == iter->pChannel)
		{
			iter = m_FadingLoopChannels.erase(iter);
			continue;
		}

		_bool bIsPlaying = false;
		const FMOD_RESULT eIsPlayingResult = iter->pChannel->isPlaying(&bIsPlaying);
		if (FMOD_OK != eIsPlayingResult || !bIsPlaying)
		{
			iter = m_FadingLoopChannels.erase(iter);
			continue;
		}

		iter->fCurrentVolume -= iter->fFadeSpeed * fTimeDelta;
		if (iter->fCurrentVolume <= 0.f)
		{
			const FMOD_RESULT eStopResult = iter->pChannel->stop();
			if (FMOD_OK != eStopResult)
				Log_FMODFailure(eStopResult, "Channel::stop", iter->strSoundTag);

			iter = m_FadingLoopChannels.erase(iter);
			continue;
		}

		const FMOD_RESULT eVolumeResult = iter->pChannel->setVolume(iter->fCurrentVolume);
		if (FMOD_OK != eVolumeResult)
		{
			Log_FMODFailure(eVolumeResult, "Channel::setVolume", iter->strSoundTag);
			iter = m_FadingLoopChannels.erase(iter);
			continue;
		}

		++iter;
	}
}

void CSoundManager::Play_Once(const _wstring& strSoundTag, const _float fVolumeScale)
{
	const SOUND_ASSET_DESC* pSoundDesc = Find_SoundDesc(strSoundTag);
	if(nullptr==pSoundDesc)
		return;

	FMOD::Channel* pChannel = nullptr;
	if(FAILED(Play_Sound(*pSoundDesc, FMOD_LOOP_OFF, fVolumeScale, &pChannel))) 
		return;
}

void CSoundManager::Play_Once_Pitched(
	const _wstring& strSoundTag,
	const _float fPitch,
	const _float fVolumeScale)
{
	const SOUND_ASSET_DESC* pSoundDesc = Find_SoundDesc(strSoundTag);
	if(nullptr==pSoundDesc)
		return;

	FMOD::Channel* pChannel = nullptr;
	if(FAILED(Play_Sound(*pSoundDesc, FMOD_LOOP_OFF, fVolumeScale, &pChannel)))
		return ;

	if(nullptr==pChannel)return;

	const FMOD_RESULT eResult = pChannel->setPitch(fPitch);
	if (FMOD_OK != eResult)
		Log_FMODFailure(eResult, "Channel::setPitch", strSoundTag);
}

void CSoundManager::Play_Loop(
	const _wstring& strSoundTag,
	const _float fVolumeScale,
	const _float fFadeOutDuration)
{
	const SOUND_ASSET_DESC* pSoundDesc = Find_SoundDesc(strSoundTag);
	if(nullptr==pSoundDesc)return;

	Stop_LoopByTag(strSoundTag, fFadeOutDuration);

	FMOD::Channel* pChannel = nullptr;
	if(FAILED(Play_Sound(*pSoundDesc, FMOD_LOOP_NORMAL, fVolumeScale, &pChannel)))
		return;
	if(nullptr==pChannel)return;

	m_ActiveLoopChannels[strSoundTag] = pChannel;
}

void CSoundManager::Stop_Sound(const _wstring& strSoundTag,float fadeOutTime)
{
	const SOUND_ASSET_DESC* pSoundDesc = Find_SoundDesc(strSoundTag);
	if (nullptr == pSoundDesc)
	{
		//LOG_WARN("Stop_Sound ignored unknown soundTag='{}'", String::WstrToStr_UTF8(strSoundTag));
		return;
	}

	Stop_LoopByTag(strSoundTag, fadeOutTime);
	//Clear_FadeEntries(strSoundTag);

	const FMOD::Sound* pTargetSound = Find_Sound(strSoundTag);
	if (nullptr == pTargetSound)
	{
		//LOG_WARN("Stop_Sound ignored unloaded soundTag='{}'", String::WstrToStr_UTF8(strSoundTag));
		return;
	}

	FMOD::ChannelGroup* pCategoryGroup = Find_CategoryGroup(pSoundDesc->eCategory);
	if(nullptr==pCategoryGroup)return;

	_int iChannelCount = 0;
	FMOD_RESULT eResult = pCategoryGroup->getNumChannels(&iChannelCount);
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "ChannelGroup::getNumChannels", strSoundTag);
		return;
	}

	vector<FMOD::Channel*> channelsToStop{};
	channelsToStop.reserve(static_cast<size_t>(max(iChannelCount, 0)));

	for (_int iChannelIndex = 0; iChannelIndex < iChannelCount; ++iChannelIndex)
	{
		FMOD::Channel* pChannel = nullptr;
		eResult = pCategoryGroup->getChannel(iChannelIndex, &pChannel);
		if (FMOD_OK != eResult || nullptr == pChannel)
			continue;

		_bool bIsPlaying = false;
		if (FMOD_OK != pChannel->isPlaying(&bIsPlaying) || !bIsPlaying)
			continue;

		FMOD::Sound* pCurrentSound = nullptr;
		if (FMOD_OK != pChannel->getCurrentSound(&pCurrentSound))
			continue;

		if (pCurrentSound == pTargetSound)
			channelsToStop.push_back(pChannel);
	}

	for (FMOD::Channel* pChannel : channelsToStop)
	{
		if (nullptr == pChannel)
			continue;

		eResult = pChannel->stop();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "Channel::stop", strSoundTag);
	}
}

void CSoundManager::Stop_AllSounds()
{
	for (FMOD::ChannelGroup* pCategoryGroup : m_pCategoryGroups)
	{
		if (nullptr == pCategoryGroup)
			continue;

		const FMOD_RESULT eResult = pCategoryGroup->stop();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "ChannelGroup::stop");
	}

	m_ActiveLoopChannels.clear();
	m_FadingLoopChannels.clear();
}

void CSoundManager::Set_SoundCategoryVolume(const SOUND_CATEGORY eCategory, const _float fVolume)
{
	FMOD::ChannelGroup* pCategoryGroup = Find_CategoryGroup(eCategory);
	if(nullptr==pCategoryGroup)return;

	const FMOD_RESULT eResult = pCategoryGroup->setVolume(ClampVolume(fVolume));
	if (FMOD_OK != eResult)
		Log_FMODFailure(eResult, "ChannelGroup::setVolume");
}

_bool CSoundManager::Is_ValidCategory(const SOUND_CATEGORY eCategory) const
{
	return static_cast<size_t>(eCategory) < s_uiSoundCategoryCount && SOUND_CATEGORY::END != eCategory;
}

_bool CSoundManager::Is_ValidLoadMode(const SOUND_LOAD_MODE eLoadMode) const
{
	return SOUND_LOAD_MODE::STATIC == eLoadMode || SOUND_LOAD_MODE::STREAM == eLoadMode;
}

FMOD::ChannelGroup* CSoundManager::Find_CategoryGroup(const SOUND_CATEGORY eCategory) const
{
	if (!Is_ValidCategory(eCategory))
	{
		//   LOG_WARN("Invalid sound category index={}", static_cast<_uint>(eCategory));
		return nullptr;
	}

	return m_pCategoryGroups[static_cast<size_t>(eCategory)];
}

const SOUND_ASSET_DESC* CSoundManager::Find_SoundDesc(const _wstring& strSoundTag) const
{
	const auto iter = m_SoundDescs.find(strSoundTag);
	if (m_SoundDescs.end() == iter)
	{
		//LOG_WARN("Unknown soundTag='{}'", WstrToStr_UTF8(strSoundTag));
		return nullptr;
	}

	return &iter->second;
}

FMOD::Sound* CSoundManager::Find_Sound(const _wstring& strSoundTag) const
{
	const auto iter = m_Sounds.find(strSoundTag);
	if (m_Sounds.end() == iter)
		return nullptr;

	return iter->second;
}

HRESULT CSoundManager::Create_Sound(const SOUND_ASSET_DESC& tSoundDesc)
{
	if(nullptr==m_pSystem)return E_FAIL;;

	const _string strAssetPathUtf8 = WstrToStr_UTF8(tSoundDesc.strAssetPath);
	if (strAssetPathUtf8.empty())
	{
	   // LOG_ERROR(
	   //     "Failed to convert sound assetPath to UTF-8. soundTag='{}'",
	   //     String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
	   // );
	    return E_FAIL;
	}

	FMOD::Sound* pSound = nullptr;
	FMOD_RESULT eResult = FMOD_OK;

	switch (tSoundDesc.eLoadMode)
	{
	case SOUND_LOAD_MODE::STATIC:
		eResult = m_pSystem->createSound(strAssetPathUtf8.c_str(), FMOD_DEFAULT, nullptr, &pSound);
		break;

	case SOUND_LOAD_MODE::STREAM:
		eResult = m_pSystem->createStream(strAssetPathUtf8.c_str(), FMOD_DEFAULT, nullptr, &pSound);
		break;

	default:
		// LOG_ERROR(
		//     "Sound creation failed. Unsupported loadMode. soundTag='{}'",
		//     String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
		// );
		return E_FAIL;
	}

	if (FMOD_OK != eResult || nullptr == pSound)
	{
		Log_FMODFailure(eResult, "System::createSound/createStream", tSoundDesc.strSoundTag);
		return E_FAIL;
	}

	m_Sounds.emplace(tSoundDesc.strSoundTag, pSound);
	return S_OK;
}

HRESULT CSoundManager::Play_Sound(
	const SOUND_ASSET_DESC& tSoundDesc,
	const FMOD_MODE eLoopMode,
	const _float fVolumeScale,
	FMOD::Channel** ppOutChannel)
{
	if(nullptr==m_pSystem)return E_FAIL;
	if(nullptr==ppOutChannel)return E_FAIL;

	*ppOutChannel = nullptr;

	FMOD::Sound* pSound = Find_Sound(tSoundDesc.strSoundTag);
	if (nullptr == pSound)
	{
		//LOG_WARN("Play ignored unloaded soundTag='{}'", String::WstrToStr_UTF8(tSoundDesc.strSoundTag));
		return E_FAIL;
	}

	FMOD::ChannelGroup* pCategoryGroup = Find_CategoryGroup(tSoundDesc.eCategory);
	if(nullptr==pCategoryGroup) return E_FAIL;

	FMOD::Channel* pChannel = nullptr;
	FMOD_RESULT eResult = m_pSystem->playSound(pSound, pCategoryGroup, true, &pChannel);
	if (FMOD_OK != eResult || nullptr == pChannel)
	{
		Log_FMODFailure(eResult, "System::playSound", tSoundDesc.strSoundTag);
		return E_FAIL;
	}

	eResult = pChannel->setMode(eLoopMode);
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "Channel::setMode", tSoundDesc.strSoundTag);
		pChannel->stop();
		return E_FAIL;
	}

	eResult = pChannel->setVolume(ClampVolume(tSoundDesc.fDefaultVolume * fVolumeScale));
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "Channel::setVolume", tSoundDesc.strSoundTag);
		pChannel->stop();
		return E_FAIL;
	}

	eResult = pChannel->setPaused(false);
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "Channel::setPaused", tSoundDesc.strSoundTag);
		pChannel->stop();
		return E_FAIL;
	}

	*ppOutChannel = pChannel;
	return S_OK;
}

void CSoundManager::Stop_LoopByTag(const _wstring& strSoundTag, const _float fFadeOutDuration)
{
	const auto iter = m_ActiveLoopChannels.find(strSoundTag);
	if (m_ActiveLoopChannels.end() == iter)
		return;

	FMOD::Channel* pChannel = iter->second;
	m_ActiveLoopChannels.erase(iter);

	if (nullptr == pChannel)
		return;

	if (fFadeOutDuration <= 0.f)
	{
		const FMOD_RESULT eResult = pChannel->stop();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "Channel::stop", strSoundTag);
		return;
	}

	_float fCurrentVolume = 0.f;
	const FMOD_RESULT eVolumeResult = pChannel->getVolume(&fCurrentVolume);
	if (FMOD_OK != eVolumeResult)
	{
		Log_FMODFailure(eVolumeResult, "Channel::getVolume", strSoundTag);
		const FMOD_RESULT eStopResult = pChannel->stop();
		if (FMOD_OK != eStopResult)
			Log_FMODFailure(eStopResult, "Channel::stop", strSoundTag);
		return;
	}

	if (fCurrentVolume <= 0.f)
	{
		const FMOD_RESULT eStopResult = pChannel->stop();
		if (FMOD_OK != eStopResult)
			Log_FMODFailure(eStopResult, "Channel::stop", strSoundTag);
		return;
	}

	Clear_FadeEntries(strSoundTag);

	LOOP_FADE_DESC tFadeDesc{};
	tFadeDesc.strSoundTag = strSoundTag;
	tFadeDesc.pChannel = pChannel;
	tFadeDesc.fCurrentVolume = fCurrentVolume;
	tFadeDesc.fFadeSpeed = fCurrentVolume / fFadeOutDuration;
	m_FadingLoopChannels.push_back(move(tFadeDesc));
}

void CSoundManager::Cleanup_FinishedLoopChannels()
{
	auto iter = m_ActiveLoopChannels.begin();
	while (iter != m_ActiveLoopChannels.end())
	{
		if (nullptr == iter->second)
		{
			iter = m_ActiveLoopChannels.erase(iter);
			continue;
		}

		_bool bIsPlaying = false;
		const FMOD_RESULT eResult = iter->second->isPlaying(&bIsPlaying);
		if (FMOD_OK != eResult || !bIsPlaying)
		{
			iter = m_ActiveLoopChannels.erase(iter);
			continue;
		}

		++iter;
	}
}

void CSoundManager::Cleanup_FinishedFadeChannels()
{
	auto iter = m_FadingLoopChannels.begin();
	while (iter != m_FadingLoopChannels.end())
	{
		if (nullptr == iter->pChannel)
		{
			iter = m_FadingLoopChannels.erase(iter);
			continue;
		}

		_bool bIsPlaying = false;
		const FMOD_RESULT eResult = iter->pChannel->isPlaying(&bIsPlaying);
		if (FMOD_OK != eResult || !bIsPlaying)
		{
			iter = m_FadingLoopChannels.erase(iter);
			continue;
		}

		++iter;
	}
}

void CSoundManager::Clear_FadeEntries(const _wstring& strSoundTag)
{
	auto iter = m_FadingLoopChannels.begin();
	while (iter != m_FadingLoopChannels.end())
	{
		if (iter->strSoundTag != strSoundTag)
		{
			++iter;
			continue;
		}

		if (nullptr != iter->pChannel)
		{
			const FMOD_RESULT eResult = iter->pChannel->stop();
			if (FMOD_OK != eResult)
				Log_FMODFailure(eResult, "Channel::stop", strSoundTag);
		}

		iter = m_FadingLoopChannels.erase(iter);
	}
}

unique_ptr<CSoundManager> CSoundManager::Create()
{
	auto upInstance = make_unique<CSoundManager>();

	if (FAILED(upInstance->Initialize())) [[unlikely]]
	{
		//LOG_CRITICAL("Failed to Create : CSoundManager");
		MSG_BOX("Failed to Create : CSoundManager");
		return nullptr;
	}

	return upInstance;
}

void CSoundManager::Free()
{
	Stop_AllSounds();

	for (auto& [strSoundTag, pSound] : m_Sounds)
	{
		if (nullptr == pSound)
			continue;

		const FMOD_RESULT eResult = pSound->release();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "Sound::release");
	}
	m_Sounds.clear();
	m_SoundDescs.clear();
	m_ActiveLoopChannels.clear();
	m_FadingLoopChannels.clear();

	for (FMOD::ChannelGroup*& pCategoryGroup : m_pCategoryGroups)
	{
		if (nullptr == pCategoryGroup)
			continue;

		const FMOD_RESULT eResult = pCategoryGroup->release();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "ChannelGroup::release");

		pCategoryGroup = nullptr;
	}

	if (nullptr != m_pSystem)
	{
		FMOD_RESULT eResult = m_pSystem->close();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "System::close");

		eResult = m_pSystem->release();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "System::release");

		m_pSystem = nullptr;
	}
}
