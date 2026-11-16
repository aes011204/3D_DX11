#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CSoundManager final : public CBase
{
private:
    using LoopFadeDesc = struct tLoopFadeDesc
    {
        _wstring soundTag{};
        FMOD::Channel* channel{};
        _float currentVolume{};
        _float fadeSpeed{};
    };

public:
    explicit CSoundManager();
    ~CSoundManager() override;

public:
    HRESULT Initialize();
    HRESULT Register_Sounds(const std::vector<SoundAssetDesc>& vecSoundDescs);
    void Update_Audio(_float timeDelta);

public:
    void Play_Once(const _wstring& soundTag, _float volumeScale = 1.f);
    void Play_Once_Pitched(const _wstring& soundTag, _float pitch, _float volumeScale = 1.f);
    void Play_Loop(const _wstring& soundTag, _float volumeScale = 1.f, _float fadeOutDuration = 0.f);
    void Stop_Sound(const _wstring& soundTag, float fadeOutTime);
    void Stop_AllSounds();
    void Set_SoundCategoryVolume(ESoundCategory category, _float volume);

private:
    FMOD::System* m_system{};
    array<FMOD::ChannelGroup*, static_cast<size_t>(ESoundCategory::END)> m_categoryGroups{};
    unordered_map<_wstring, SoundAssetDesc> m_soundDescs{};
    unordered_map<_wstring, FMOD::Sound*> m_sounds{};
    unordered_map<_wstring, FMOD::Channel*> m_activeLoopChannels{};
    vector<LoopFadeDesc> m_fadingLoopChannels{};

private:
    _bool Is_ValidCategory(ESoundCategory category) const;
    _bool Is_ValidLoadMode(ESoundLoadMode loadMode) const;
    FMOD::ChannelGroup* Find_CategoryGroup(ESoundCategory category) const;
    const SoundAssetDesc* Find_SoundDesc(const _wstring& soundTag) const;
    FMOD::Sound* Find_Sound(const _wstring& soundTag) const;

    HRESULT Create_Sound(const SoundAssetDesc& tSoundDesc);
    HRESULT Play_Sound(
        const SoundAssetDesc& tSoundDesc,
        FMOD_MODE loopMode,
        _float volumeScale,
        FMOD::Channel** ppOutChannel);

    void Stop_LoopByTag(const _wstring& soundTag, _float fadeOutDuration);
    void Cleanup_FinishedLoopChannels();
    void Cleanup_FinishedFadeChannels();
    void Clear_FadeEntries(const _wstring& soundTag);

public:
    static unique_ptr<CSoundManager> Create();
    void Free() override;
};

NS_END
