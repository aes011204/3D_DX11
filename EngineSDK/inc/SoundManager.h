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
    void Update_Audio(_float fTimeDelta);

public:
    void Play_Once(const _wstring& strSoundTag, _float fVolumeScale = 1.f);
    void Play_Once_Pitched(const _wstring& strSoundTag, _float fPitch, _float fVolumeScale = 1.f);
    void Play_Loop(const _wstring& strSoundTag, _float fVolumeScale = 1.f, _float fFadeOutDuration = 0.f);
    void Stop_Sound(const _wstring& strSoundTag, float fadeOutTime);
    void Stop_AllSounds();
    void Set_SoundCategoryVolume(ESoundCategory eCategory, _float fVolume);

private:
    FMOD::System* m_system{};
    array<FMOD::ChannelGroup*, static_cast<size_t>(ESoundCategory::END)> m_categoryGroups{};
    unordered_map<_wstring, SoundAssetDesc> m_soundDescs{};
    unordered_map<_wstring, FMOD::Sound*> m_sounds{};
    unordered_map<_wstring, FMOD::Channel*> m_activeLoopChannels{};
    vector<LoopFadeDesc> m_fadingLoopChannels{};

private:
    _bool Is_ValidCategory(ESoundCategory eCategory) const;
    _bool Is_ValidLoadMode(ESoundLoadMode eLoadMode) const;
    FMOD::ChannelGroup* Find_CategoryGroup(ESoundCategory eCategory) const;
    const SoundAssetDesc* Find_SoundDesc(const _wstring& strSoundTag) const;
    FMOD::Sound* Find_Sound(const _wstring& strSoundTag) const;

    HRESULT Create_Sound(const SoundAssetDesc& tSoundDesc);
    HRESULT Play_Sound(
        const SoundAssetDesc& tSoundDesc,
        FMOD_MODE eLoopMode,
        _float fVolumeScale,
        FMOD::Channel** ppOutChannel);

    void Stop_LoopByTag(const _wstring& strSoundTag, _float fFadeOutDuration);
    void Cleanup_FinishedLoopChannels();
    void Cleanup_FinishedFadeChannels();
    void Clear_FadeEntries(const _wstring& strSoundTag);

public:
    static unique_ptr<CSoundManager> Create();
    void Free() override;
};

NS_END
