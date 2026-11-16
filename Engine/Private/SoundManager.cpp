
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
		const int requiredSize = WideCharToMultiByte(
			CP_UTF8,
			0,
			wstrText.data(),
			static_cast<int>(wstrText.size()),
			nullptr,
			0,
			nullptr,
			nullptr
		);

		if (requiredSize <= 0)
			return {};

		_string text;
		text.resize(static_cast<size_t>(requiredSize));

		const int convertedSize = WideCharToMultiByte(
			CP_UTF8,
			0,
			wstrText.data(),
			static_cast<int>(wstrText.size()),
			text.data(),
			requiredSize,
			nullptr,
			nullptr
		);

		if (convertedSize <= 0)
			return {};

		return text;
	}

	
	[[nodiscard]] inline _string CharToStr(const _char* text)
	{
		return nullptr != text ? _string{ text } : _string{};
	}

	
	[[nodiscard]] inline _wstring TCharToWstr(const _tchar* text)
	{
		return nullptr != text ? _wstring{ text } : _wstring{};
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
		const FMOD_RESULT result,
		const _char* operation,
		const _wstring& soundTag = {})
	{
		//const _char* pErrorText = FMOD_ErrorString(eResult);
		if (soundTag.empty())
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

	_float ClampVolume(const _float volume)
	{
		return clamp(volume, 0.f, 1.f);
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
	FMOD_RESULT result = FMOD::System_Create(&m_system);
	if (FMOD_OK != result)
	{
		Log_FMODFailure(result, "System_Create");
		return E_FAIL;
	}

	result = m_system->init(32, FMOD_INIT_NORMAL, nullptr);
	if (FMOD_OK != result)
	{
		Log_FMODFailure(result, "System::init");
		return E_FAIL;
	}

	for (size_t uiIndex = 0; uiIndex < s_uiSoundCategoryCount; ++uiIndex)
	{
		result = m_system->createChannelGroup(
			s_CategoryGroupNames[uiIndex],
			&m_categoryGroups[uiIndex]
		);
		if (FMOD_OK != result)
		{
			Log_FMODFailure(result, "System::createChannelGroup");
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

void CSoundManager::Update_Audio(const _float timeDelta)
{
	if (nullptr == m_system)
		return;

	const FMOD_RESULT result = m_system->update();
	if (FMOD_OK != result)
		Log_FMODFailure(result, "System::update");

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

		_bool isPlaying = false;
		const FMOD_RESULT isPlayingResult = iter->channel->isPlaying(&isPlaying);
		if (FMOD_OK != isPlayingResult || !isPlaying)
		{
			iter = m_fadingLoopChannels.erase(iter);
			continue;
		}

		iter->currentVolume -= iter->fadeSpeed * timeDelta;
		if (iter->currentVolume <= 0.f)
		{
			const FMOD_RESULT stopResult = iter->channel->stop();
			if (FMOD_OK != stopResult)
				Log_FMODFailure(stopResult, "Channel::stop", iter->soundTag);

			iter = m_fadingLoopChannels.erase(iter);
			continue;
		}

		const FMOD_RESULT volumeResult = iter->channel->setVolume(iter->currentVolume);
		if (FMOD_OK != volumeResult)
		{
			Log_FMODFailure(volumeResult, "Channel::setVolume", iter->soundTag);
			iter = m_fadingLoopChannels.erase(iter);
			continue;
		}

		++iter;
	}
}

void CSoundManager::Play_Once(const _wstring& soundTag, const _float volumeScale)
{
	const SoundAssetDesc* soundDesc = Find_SoundDesc(soundTag);
	if(nullptr==soundDesc)
		return;

	FMOD::Channel* channel = nullptr;
	if(FAILED(Play_Sound(*soundDesc, FMOD_LOOP_OFF, volumeScale, &channel))) 
		return;
}

void CSoundManager::Play_Once_Pitched(
	const _wstring& soundTag,
	const _float pitch,
	const _float volumeScale)
{
	const SoundAssetDesc* soundDesc = Find_SoundDesc(soundTag);
	if(nullptr==soundDesc)
		return;

	FMOD::Channel* channel = nullptr;
	if(FAILED(Play_Sound(*soundDesc, FMOD_LOOP_OFF, volumeScale, &channel)))
		return ;

	if(nullptr==channel)return;

	const FMOD_RESULT result = channel->setPitch(pitch);
	if (FMOD_OK != result)
		Log_FMODFailure(result, "Channel::setPitch", soundTag);
}

void CSoundManager::Play_Loop(
	const _wstring& soundTag,
	const _float volumeScale,
	const _float fadeOutDuration)
{
	const SoundAssetDesc* soundDesc = Find_SoundDesc(soundTag);
	if(nullptr==soundDesc)return;

	Stop_LoopByTag(soundTag, fadeOutDuration);

	FMOD::Channel* channel = nullptr;
	if(FAILED(Play_Sound(*soundDesc, FMOD_LOOP_NORMAL, volumeScale, &channel)))
		return;
	if(nullptr==channel)return;

	m_activeLoopChannels[soundTag] = channel;
}

void CSoundManager::Stop_Sound(const _wstring& soundTag,float fadeOutTime)
{
	const SoundAssetDesc* soundDesc = Find_SoundDesc(soundTag);
	if (nullptr == soundDesc)
	{
		//LOG_WARN("Stop_Sound ignored unknown soundTag='{}'", String::WstrToStr_UTF8(strSoundTag));
		return;
	}

	Stop_LoopByTag(soundTag, fadeOutTime);
	//Clear_FadeEntries(strSoundTag);

	const FMOD::Sound* targetSound = Find_Sound(soundTag);
	if (nullptr == targetSound)
	{
		//LOG_WARN("Stop_Sound ignored unloaded soundTag='{}'", String::WstrToStr_UTF8(strSoundTag));
		return;
	}

	FMOD::ChannelGroup* categoryGroup = Find_CategoryGroup(soundDesc->category);
	if(nullptr==categoryGroup)return;

	_int channelCount = 0;
	FMOD_RESULT result = categoryGroup->getNumChannels(&channelCount);
	if (FMOD_OK != result)
	{
		Log_FMODFailure(result, "ChannelGroup::getNumChannels", soundTag);
		return;
	}

	vector<FMOD::Channel*> channelsToStop{};
	channelsToStop.reserve(static_cast<size_t>(max(channelCount, 0)));

	for (_int channelIndex = 0; channelIndex < channelCount; ++channelIndex)
	{
		FMOD::Channel* channel = nullptr;
		result = categoryGroup->getChannel(channelIndex, &channel);
		if (FMOD_OK != result || nullptr == channel)
			continue;

		_bool isPlaying = false;
		if (FMOD_OK != channel->isPlaying(&isPlaying) || !isPlaying)
			continue;

		FMOD::Sound* currentSound = nullptr;
		if (FMOD_OK != channel->getCurrentSound(&currentSound))
			continue;

		if (currentSound == targetSound)
			channelsToStop.push_back(channel);
	}

	for (FMOD::Channel* channel : channelsToStop)
	{
		if (nullptr == channel)
			continue;

		result = channel->stop();
		if (FMOD_OK != result)
			Log_FMODFailure(result, "Channel::stop", soundTag);
	}
}

void CSoundManager::Stop_AllSounds()
{
	for (FMOD::ChannelGroup* categoryGroup : m_categoryGroups)
	{
		if (nullptr == categoryGroup)
			continue;

		const FMOD_RESULT result = categoryGroup->stop();
		if (FMOD_OK != result)
			Log_FMODFailure(result, "ChannelGroup::stop");
	}

	m_activeLoopChannels.clear();
	m_fadingLoopChannels.clear();
}

void CSoundManager::Set_SoundCategoryVolume(const ESoundCategory category, const _float volume)
{
	FMOD::ChannelGroup* categoryGroup = Find_CategoryGroup(category);
	if(nullptr==categoryGroup)return;

	const FMOD_RESULT result = categoryGroup->setVolume(ClampVolume(volume));
	if (FMOD_OK != result)
		Log_FMODFailure(result, "ChannelGroup::setVolume");
}

_bool CSoundManager::Is_ValidCategory(const ESoundCategory category) const
{
	return static_cast<size_t>(category) < s_uiSoundCategoryCount && ESoundCategory::END != category;
}

_bool CSoundManager::Is_ValidLoadMode(const ESoundLoadMode loadMode) const
{
	return ESoundLoadMode::STATIC == loadMode || ESoundLoadMode::STREAM == loadMode;
}

FMOD::ChannelGroup* CSoundManager::Find_CategoryGroup(const ESoundCategory category) const
{
	if (!Is_ValidCategory(category))
	{
		//   LOG_WARN("Invalid sound category index={}", static_cast<_uint>(eCategory));
		return nullptr;
	}

	return m_categoryGroups[static_cast<size_t>(category)];
}

const SoundAssetDesc* CSoundManager::Find_SoundDesc(const _wstring& soundTag) const
{
	const auto iter = m_soundDescs.find(soundTag);
	if (m_soundDescs.end() == iter)
	{
		//LOG_WARN("Unknown soundTag='{}'", WstrToStr_UTF8(strSoundTag));
		return nullptr;
	}

	return &iter->second;
}

FMOD::Sound* CSoundManager::Find_Sound(const _wstring& soundTag) const
{
	const auto iter = m_sounds.find(soundTag);
	if (m_sounds.end() == iter)
		return nullptr;

	return iter->second;
}

HRESULT CSoundManager::Create_Sound(const SoundAssetDesc& tSoundDesc)
{
	if(nullptr==m_system)return E_FAIL;;

	const _string assetPathUtf8 = WstrToStr_UTF8(tSoundDesc.assetPath);
	if (assetPathUtf8.empty())
	{
	   // LOG_ERROR(
	   //     "Failed to convert sound assetPath to UTF-8. soundTag='{}'",
	   //     String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
	   // );
	    return E_FAIL;
	}

	FMOD::Sound* sound = nullptr;
	FMOD_RESULT result = FMOD_OK;

	switch (tSoundDesc.loadMode)
	{
	case ESoundLoadMode::STATIC:
		result = m_system->createSound(assetPathUtf8.c_str(), FMOD_DEFAULT, nullptr, &sound);
		break;

	case ESoundLoadMode::STREAM:
		result = m_system->createStream(assetPathUtf8.c_str(), FMOD_DEFAULT, nullptr, &sound);
		break;

	default:
		// LOG_ERROR(
		//     "Sound creation failed. Unsupported loadMode. soundTag='{}'",
		//     String::WstrToStr_UTF8(tSoundDesc.strSoundTag)
		// );
		return E_FAIL;
	}

	if (FMOD_OK != result || nullptr == sound)
	{
		Log_FMODFailure(result, "System::createSound/createStream", tSoundDesc.soundTag);
		return E_FAIL;
	}

	m_sounds.emplace(tSoundDesc.soundTag, sound);
	return S_OK;
}

HRESULT CSoundManager::Play_Sound(
	const SoundAssetDesc& tSoundDesc,
	const FMOD_MODE loopMode,
	const _float volumeScale,
	FMOD::Channel** ppOutChannel)
{
	if(nullptr==m_system)return E_FAIL;
	if(nullptr==ppOutChannel)return E_FAIL;

	*ppOutChannel = nullptr;

	FMOD::Sound* sound = Find_Sound(tSoundDesc.soundTag);
	if (nullptr == sound)
	{
		//LOG_WARN("Play ignored unloaded soundTag='{}'", String::WstrToStr_UTF8(tSoundDesc.strSoundTag));
		return E_FAIL;
	}

	FMOD::ChannelGroup* categoryGroup = Find_CategoryGroup(tSoundDesc.category);
	if(nullptr==categoryGroup) return E_FAIL;

	FMOD::Channel* channel = nullptr;
	FMOD_RESULT result = m_system->playSound(sound, categoryGroup, true, &channel);
	if (FMOD_OK != result || nullptr == channel)
	{
		Log_FMODFailure(result, "System::playSound", tSoundDesc.soundTag);
		return E_FAIL;
	}

	result = channel->setMode(loopMode);
	if (FMOD_OK != result)
	{
		Log_FMODFailure(result, "Channel::setMode", tSoundDesc.soundTag);
		channel->stop();
		return E_FAIL;
	}

	result = channel->setVolume(ClampVolume(tSoundDesc.defaultVolume * volumeScale));
	if (FMOD_OK != result)
	{
		Log_FMODFailure(result, "Channel::setVolume", tSoundDesc.soundTag);
		channel->stop();
		return E_FAIL;
	}

	result = channel->setPaused(false);
	if (FMOD_OK != result)
	{
		Log_FMODFailure(result, "Channel::setPaused", tSoundDesc.soundTag);
		channel->stop();
		return E_FAIL;
	}

	*ppOutChannel = channel;
	return S_OK;
}

void CSoundManager::Stop_LoopByTag(const _wstring& soundTag, const _float fadeOutDuration)
{
	const auto iter = m_activeLoopChannels.find(soundTag);
	if (m_activeLoopChannels.end() == iter)
		return;

	FMOD::Channel* channel = iter->second;
	m_activeLoopChannels.erase(iter);

	if (nullptr == channel)
		return;

	if (fadeOutDuration <= 0.f)
	{
		const FMOD_RESULT result = channel->stop();
		if (FMOD_OK != result)
			Log_FMODFailure(result, "Channel::stop", soundTag);
		return;
	}

	_float currentVolume = 0.f;
	const FMOD_RESULT volumeResult = channel->getVolume(&currentVolume);
	if (FMOD_OK != volumeResult)
	{
		Log_FMODFailure(volumeResult, "Channel::getVolume", soundTag);
		const FMOD_RESULT stopResult = channel->stop();
		if (FMOD_OK != stopResult)
			Log_FMODFailure(stopResult, "Channel::stop", soundTag);
		return;
	}

	if (currentVolume <= 0.f)
	{
		const FMOD_RESULT stopResult = channel->stop();
		if (FMOD_OK != stopResult)
			Log_FMODFailure(stopResult, "Channel::stop", soundTag);
		return;
	}

	Clear_FadeEntries(soundTag);

	LoopFadeDesc tFadeDesc{};
	tFadeDesc.soundTag = soundTag;
	tFadeDesc.channel = channel;
	tFadeDesc.currentVolume = currentVolume;
	tFadeDesc.fadeSpeed = currentVolume / fadeOutDuration;
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

		_bool isPlaying = false;
		const FMOD_RESULT result = iter->second->isPlaying(&isPlaying);
		if (FMOD_OK != result || !isPlaying)
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

		_bool isPlaying = false;
		const FMOD_RESULT result = iter->channel->isPlaying(&isPlaying);
		if (FMOD_OK != result || !isPlaying)
		{
			iter = m_fadingLoopChannels.erase(iter);
			continue;
		}

		++iter;
	}
}

void CSoundManager::Clear_FadeEntries(const _wstring& soundTag)
{
	auto iter = m_fadingLoopChannels.begin();
	while (iter != m_fadingLoopChannels.end())
	{
		if (iter->soundTag != soundTag)
		{
			++iter;
			continue;
		}

		if (nullptr != iter->channel)
		{
			const FMOD_RESULT result = iter->channel->stop();
			if (FMOD_OK != result)
				Log_FMODFailure(result, "Channel::stop", soundTag);
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

	for (auto& [soundTag, sound] : m_sounds)
	{
		if (nullptr == sound)
			continue;

		const FMOD_RESULT result = sound->release();
		if (FMOD_OK != result)
			Log_FMODFailure(result, "Sound::release");
	}
	m_sounds.clear();
	m_soundDescs.clear();
	m_activeLoopChannels.clear();
	m_fadingLoopChannels.clear();

	for (FMOD::ChannelGroup*& categoryGroup : m_categoryGroups)
	{
		if (nullptr == categoryGroup)
			continue;

		const FMOD_RESULT result = categoryGroup->release();
		if (FMOD_OK != result)
			Log_FMODFailure(result, "ChannelGroup::release");

		categoryGroup = nullptr;
	}

	if (nullptr != m_system)
	{
		FMOD_RESULT result = m_system->close();
		if (FMOD_OK != result)
			Log_FMODFailure(result, "System::close");

		result = m_system->release();
		if (FMOD_OK != result)
			Log_FMODFailure(result, "System::release");

		m_system = nullptr;
	}
}
