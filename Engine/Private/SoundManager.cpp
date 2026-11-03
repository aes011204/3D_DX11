
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

	constexpr size_t s_uiSoundCategoryCount = static_cast<size_t>(ESoundCategory::END);

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
	FMOD_RESULT eResult = FMOD::System_Create(&m_system);
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "System_Create");
		return E_FAIL;
	}

	eResult = m_system->init(32, FMOD_INIT_NORMAL, nullptr);
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "System::init");
		return E_FAIL;
	}

	for (size_t uiIndex = 0; uiIndex < s_uiSoundCategoryCount; ++uiIndex)
	{
		eResult = m_system->createChannelGroup(
			s_CategoryGroupNames[uiIndex],
			&m_categoryGroups[uiIndex]
		);
		if (FMOD_OK != eResult)
		{
			Log_FMODFailure(eResult, "System::createChannelGroup");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSoundManager::Register_Sounds(const vector<SoundAssetDesc>& vecSoundDescs)
{
	if(nullptr==m_system)return E_FAIL;

	for (const SoundAssetDesc& tSoundDesc : vecSoundDescs)
	{
		if (tSoundDesc.soundTag.empty())
		{
			//LOG_ERROR("Sound registration failed. soundTag must not be empty.");
			return E_FAIL;
		}

		if (tSoundDesc.assetPath.empty())
		{
			//LOG_ERROR(
			//    "Sound registration failed. assetPath must not be empty. soundTag='{}'",
			//    String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
			//);
			return E_FAIL;
		}

		if (!Is_ValidCategory(tSoundDesc.category))
		{
			//LOG_ERROR(
			//    "Sound registration failed. Invalid category. soundTag='{}'",
			//    String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
			//);
			return E_FAIL;
		}

		if (!Is_ValidLoadMode(tSoundDesc.loadMode))
		{
			// LOG_ERROR(
			//     "Sound registration failed. Invalid loadMode. soundTag='{}'",
			//     String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
			// );
			return E_FAIL;
		}

		if (0.f > tSoundDesc.defaultVolume || 1.f < tSoundDesc.defaultVolume)
		{
			//LOG_ERROR(
			//    "Sound registration failed. defaultVolume must be in [0, 1]. soundTag='{}' Volume={}",
			//    String::WstrToStr_UTF8(tSoundDesc.strSoundTag),
			//    tSoundDesc.fDefaultVolume
			//);
			return E_FAIL;
		}

		if (m_soundDescs.contains(tSoundDesc.soundTag))
		{
			//LOG_ERROR(
			//    "Sound registration failed. Duplicate soundTag='{}'",
			//    String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
			//);
			return E_FAIL;
		}

		if (FAILED(Create_Sound(tSoundDesc)))
			return E_FAIL;

		m_soundDescs.emplace(tSoundDesc.soundTag, tSoundDesc);
	}

	return S_OK;
}

void CSoundManager::Update_Audio(const _float fTimeDelta)
{
	if (nullptr == m_system)
		return;

	const FMOD_RESULT eResult = m_system->update();
	if (FMOD_OK != eResult)
		Log_FMODFailure(eResult, "System::update");

	Cleanup_FinishedLoopChannels();
	Cleanup_FinishedFadeChannels();

	auto iter = m_fadingLoopChannels.begin();
	while (iter != m_fadingLoopChannels.end())
	{
		if (nullptr == iter->channel)
		{
			iter = m_fadingLoopChannels.erase(iter);
			continue;
		}

		_bool bIsPlaying = false;
		const FMOD_RESULT eIsPlayingResult = iter->channel->isPlaying(&bIsPlaying);
		if (FMOD_OK != eIsPlayingResult || !bIsPlaying)
		{
			iter = m_fadingLoopChannels.erase(iter);
			continue;
		}

		iter->currentVolume -= iter->fadeSpeed * fTimeDelta;
		if (iter->currentVolume <= 0.f)
		{
			const FMOD_RESULT eStopResult = iter->channel->stop();
			if (FMOD_OK != eStopResult)
				Log_FMODFailure(eStopResult, "Channel::stop", iter->soundTag);

			iter = m_fadingLoopChannels.erase(iter);
			continue;
		}

		const FMOD_RESULT eVolumeResult = iter->channel->setVolume(iter->currentVolume);
		if (FMOD_OK != eVolumeResult)
		{
			Log_FMODFailure(eVolumeResult, "Channel::setVolume", iter->soundTag);
			iter = m_fadingLoopChannels.erase(iter);
			continue;
		}

		++iter;
	}
}

void CSoundManager::Play_Once(const _wstring& strSoundTag, const _float fVolumeScale)
{
	const SoundAssetDesc* pSoundDesc = Find_SoundDesc(strSoundTag);
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
	const SoundAssetDesc* pSoundDesc = Find_SoundDesc(strSoundTag);
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
	const SoundAssetDesc* pSoundDesc = Find_SoundDesc(strSoundTag);
	if(nullptr==pSoundDesc)return;

	Stop_LoopByTag(strSoundTag, fFadeOutDuration);

	FMOD::Channel* pChannel = nullptr;
	if(FAILED(Play_Sound(*pSoundDesc, FMOD_LOOP_NORMAL, fVolumeScale, &pChannel)))
		return;
	if(nullptr==pChannel)return;

	m_activeLoopChannels[strSoundTag] = pChannel;
}

void CSoundManager::Stop_Sound(const _wstring& strSoundTag,float fadeOutTime)
{
	const SoundAssetDesc* pSoundDesc = Find_SoundDesc(strSoundTag);
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

	FMOD::ChannelGroup* pCategoryGroup = Find_CategoryGroup(pSoundDesc->category);
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
	for (FMOD::ChannelGroup* pCategoryGroup : m_categoryGroups)
	{
		if (nullptr == pCategoryGroup)
			continue;

		const FMOD_RESULT eResult = pCategoryGroup->stop();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "ChannelGroup::stop");
	}

	m_activeLoopChannels.clear();
	m_fadingLoopChannels.clear();
}

void CSoundManager::Set_SoundCategoryVolume(const ESoundCategory eCategory, const _float fVolume)
{
	FMOD::ChannelGroup* pCategoryGroup = Find_CategoryGroup(eCategory);
	if(nullptr==pCategoryGroup)return;

	const FMOD_RESULT eResult = pCategoryGroup->setVolume(ClampVolume(fVolume));
	if (FMOD_OK != eResult)
		Log_FMODFailure(eResult, "ChannelGroup::setVolume");
}

_bool CSoundManager::Is_ValidCategory(const ESoundCategory eCategory) const
{
	return static_cast<size_t>(eCategory) < s_uiSoundCategoryCount && ESoundCategory::END != eCategory;
}

_bool CSoundManager::Is_ValidLoadMode(const ESoundLoadMode eLoadMode) const
{
	return ESoundLoadMode::STATIC == eLoadMode || ESoundLoadMode::STREAM == eLoadMode;
}

FMOD::ChannelGroup* CSoundManager::Find_CategoryGroup(const ESoundCategory eCategory) const
{
	if (!Is_ValidCategory(eCategory))
	{
		//   LOG_WARN("Invalid sound category index={}", static_cast<_uint>(eCategory));
		return nullptr;
	}

	return m_categoryGroups[static_cast<size_t>(eCategory)];
}

const SoundAssetDesc* CSoundManager::Find_SoundDesc(const _wstring& strSoundTag) const
{
	const auto iter = m_soundDescs.find(strSoundTag);
	if (m_soundDescs.end() == iter)
	{
		//LOG_WARN("Unknown soundTag='{}'", WstrToStr_UTF8(strSoundTag));
		return nullptr;
	}

	return &iter->second;
}

FMOD::Sound* CSoundManager::Find_Sound(const _wstring& strSoundTag) const
{
	const auto iter = m_sounds.find(strSoundTag);
	if (m_sounds.end() == iter)
		return nullptr;

	return iter->second;
}

HRESULT CSoundManager::Create_Sound(const SoundAssetDesc& tSoundDesc)
{
	if(nullptr==m_system)return E_FAIL;;

	const _string strAssetPathUtf8 = WstrToStr_UTF8(tSoundDesc.assetPath);
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

	switch (tSoundDesc.loadMode)
	{
	case ESoundLoadMode::STATIC:
		eResult = m_system->createSound(strAssetPathUtf8.c_str(), FMOD_DEFAULT, nullptr, &pSound);
		break;

	case ESoundLoadMode::STREAM:
		eResult = m_system->createStream(strAssetPathUtf8.c_str(), FMOD_DEFAULT, nullptr, &pSound);
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
		Log_FMODFailure(eResult, "System::createSound/createStream", tSoundDesc.soundTag);
		return E_FAIL;
	}

	m_sounds.emplace(tSoundDesc.soundTag, pSound);
	return S_OK;
}

HRESULT CSoundManager::Play_Sound(
	const SoundAssetDesc& tSoundDesc,
	const FMOD_MODE eLoopMode,
	const _float fVolumeScale,
	FMOD::Channel** ppOutChannel)
{
	if(nullptr==m_system)return E_FAIL;
	if(nullptr==ppOutChannel)return E_FAIL;

	*ppOutChannel = nullptr;

	FMOD::Sound* pSound = Find_Sound(tSoundDesc.soundTag);
	if (nullptr == pSound)
	{
		//LOG_WARN("Play ignored unloaded soundTag='{}'", String::WstrToStr_UTF8(tSoundDesc.strSoundTag));
		return E_FAIL;
	}

	FMOD::ChannelGroup* pCategoryGroup = Find_CategoryGroup(tSoundDesc.category);
	if(nullptr==pCategoryGroup) return E_FAIL;

	FMOD::Channel* pChannel = nullptr;
	FMOD_RESULT eResult = m_system->playSound(pSound, pCategoryGroup, true, &pChannel);
	if (FMOD_OK != eResult || nullptr == pChannel)
	{
		Log_FMODFailure(eResult, "System::playSound", tSoundDesc.soundTag);
		return E_FAIL;
	}

	eResult = pChannel->setMode(eLoopMode);
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "Channel::setMode", tSoundDesc.soundTag);
		pChannel->stop();
		return E_FAIL;
	}

	eResult = pChannel->setVolume(ClampVolume(tSoundDesc.defaultVolume * fVolumeScale));
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "Channel::setVolume", tSoundDesc.soundTag);
		pChannel->stop();
		return E_FAIL;
	}

	eResult = pChannel->setPaused(false);
	if (FMOD_OK != eResult)
	{
		Log_FMODFailure(eResult, "Channel::setPaused", tSoundDesc.soundTag);
		pChannel->stop();
		return E_FAIL;
	}

	*ppOutChannel = pChannel;
	return S_OK;
}

void CSoundManager::Stop_LoopByTag(const _wstring& strSoundTag, const _float fFadeOutDuration)
{
	const auto iter = m_activeLoopChannels.find(strSoundTag);
	if (m_activeLoopChannels.end() == iter)
		return;

	FMOD::Channel* pChannel = iter->second;
	m_activeLoopChannels.erase(iter);

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

	LoopFadeDesc tFadeDesc{};
	tFadeDesc.soundTag = strSoundTag;
	tFadeDesc.channel = pChannel;
	tFadeDesc.currentVolume = fCurrentVolume;
	tFadeDesc.fadeSpeed = fCurrentVolume / fFadeOutDuration;
	m_fadingLoopChannels.push_back(move(tFadeDesc));
}

void CSoundManager::Cleanup_FinishedLoopChannels()
{
	auto iter = m_activeLoopChannels.begin();
	while (iter != m_activeLoopChannels.end())
	{
		if (nullptr == iter->second)
		{
			iter = m_activeLoopChannels.erase(iter);
			continue;
		}

		_bool bIsPlaying = false;
		const FMOD_RESULT eResult = iter->second->isPlaying(&bIsPlaying);
		if (FMOD_OK != eResult || !bIsPlaying)
		{
			iter = m_activeLoopChannels.erase(iter);
			continue;
		}

		++iter;
	}
}

void CSoundManager::Cleanup_FinishedFadeChannels()
{
	auto iter = m_fadingLoopChannels.begin();
	while (iter != m_fadingLoopChannels.end())
	{
		if (nullptr == iter->channel)
		{
			iter = m_fadingLoopChannels.erase(iter);
			continue;
		}

		_bool bIsPlaying = false;
		const FMOD_RESULT eResult = iter->channel->isPlaying(&bIsPlaying);
		if (FMOD_OK != eResult || !bIsPlaying)
		{
			iter = m_fadingLoopChannels.erase(iter);
			continue;
		}

		++iter;
	}
}

void CSoundManager::Clear_FadeEntries(const _wstring& strSoundTag)
{
	auto iter = m_fadingLoopChannels.begin();
	while (iter != m_fadingLoopChannels.end())
	{
		if (iter->soundTag != strSoundTag)
		{
			++iter;
			continue;
		}

		if (nullptr != iter->channel)
		{
			const FMOD_RESULT eResult = iter->channel->stop();
			if (FMOD_OK != eResult)
				Log_FMODFailure(eResult, "Channel::stop", strSoundTag);
		}

		iter = m_fadingLoopChannels.erase(iter);
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

	for (auto& [strSoundTag, pSound] : m_sounds)
	{
		if (nullptr == pSound)
			continue;

		const FMOD_RESULT eResult = pSound->release();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "Sound::release");
	}
	m_sounds.clear();
	m_soundDescs.clear();
	m_activeLoopChannels.clear();
	m_fadingLoopChannels.clear();

	for (FMOD::ChannelGroup*& pCategoryGroup : m_categoryGroups)
	{
		if (nullptr == pCategoryGroup)
			continue;

		const FMOD_RESULT eResult = pCategoryGroup->release();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "ChannelGroup::release");

		pCategoryGroup = nullptr;
	}

	if (nullptr != m_system)
	{
		FMOD_RESULT eResult = m_system->close();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "System::close");

		eResult = m_system->release();
		if (FMOD_OK != eResult)
			Log_FMODFailure(eResult, "System::release");

		m_system = nullptr;
	}
}
