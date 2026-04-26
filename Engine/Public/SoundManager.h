#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CSoundManager final : public CBase
{
private:
    using LOOP_FADE_DESC = struct tLoopFadeDesc
    {
        _wstring strSoundTag{};
        FMOD::Channel* pChannel{};
        _float fCurrentVolume{};
        _float fFadeSpeed{};
    };

public:
    explicit CSoundManager();
    ~CSoundManager() override;

public:
    HRESULT Initialize();
    HRESULT Register_Sounds(const std::vector<SOUND_ASSET_DESC>& vecSoundDescs);
    void Update_Audio(_float fTimeDelta);

public:
    void Play_Once(const _wstring& strSoundTag, _float fVolumeScale = 1.f);
    void Play_Once_Pitched(const _wstring& strSoundTag, _float fPitch, _float fVolumeScale = 1.f);
    void Play_Loop(const _wstring& strSoundTag, _float fVolumeScale = 1.f, _float fFadeOutDuration = 0.f);
    void Stop_Sound(const _wstring& strSoundTag, float fadeOutTime);
    void Stop_AllSounds();
    void Set_SoundCategoryVolume(SOUND_CATEGORY eCategory, _float fVolume);

private:
    FMOD::System* m_pSystem{};
    array<FMOD::ChannelGroup*, static_cast<size_t>(SOUND_CATEGORY::END)> m_pCategoryGroups{};
    unordered_map<_wstring, SOUND_ASSET_DESC> m_SoundDescs{};
    unordered_map<_wstring, FMOD::Sound*> m_Sounds{};
    unordered_map<_wstring, FMOD::Channel*> m_ActiveLoopChannels{};
    vector<LOOP_FADE_DESC> m_FadingLoopChannels{};

private:
    _bool Is_ValidCategory(SOUND_CATEGORY eCategory) const;
    _bool Is_ValidLoadMode(SOUND_LOAD_MODE eLoadMode) const;
    FMOD::ChannelGroup* Find_CategoryGroup(SOUND_CATEGORY eCategory) const;
    const SOUND_ASSET_DESC* Find_SoundDesc(const _wstring& strSoundTag) const;
    FMOD::Sound* Find_Sound(const _wstring& strSoundTag) const;

    HRESULT Create_Sound(const SOUND_ASSET_DESC& tSoundDesc);
    HRESULT Play_Sound(
        const SOUND_ASSET_DESC& tSoundDesc,
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
