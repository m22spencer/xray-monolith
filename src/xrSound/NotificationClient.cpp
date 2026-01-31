#include "stdafx.h"
#include "NotificationClient.h"

#include "SoundRender_CoreA.h"
#include "SoundRender_Emitter.h"
#include "SoundRender_TargetA.h"
#include <AL/al.h>

CNotificationClient::CNotificationClient()
    : m_bComInitialized(false)
{
    Start();
}

CNotificationClient::~CNotificationClient() 
{
    Close();
}

inline bool CNotificationClient::Start() 
{
    // Initialize the COM library for the current thread
    HRESULT ihr = CoInitialize(NULL);

    // RPC_E_CHANGED_MODE means COM already initialized in different mode - this is OK
    if (SUCCEEDED(ihr) || ihr == RPC_E_CHANGED_MODE) {
        m_bComInitialized = SUCCEEDED(ihr);

        // Create the device enumerator
        IMMDeviceEnumerator* pEnumerator;
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
        if (SUCCEEDED(hr)) {
            // Register for device change notifications
            hr = pEnumerator->RegisterEndpointNotificationCallback(this);
            m_pEnumerator = pEnumerator;

            return true;
        }

        if (m_bComInitialized) {
            CoUninitialize();
            m_bComInitialized = false;
        }
    }

    return false;
}

inline void CNotificationClient::Close() 
{
    // Unregister the device enumerator
    if (m_pEnumerator) {
        m_pEnumerator->UnregisterEndpointNotificationCallback(this);
        m_pEnumerator->Release();
    }

    if (m_bComInitialized) {
        CoUninitialize();
        m_bComInitialized = false;
    }
}

inline STDMETHODIMP_(HRESULT __stdcall) CNotificationClient::OnDeviceAdded(LPCWSTR pwstrDeviceId) 
{
    // A new audio device has been added.
    return S_OK;
}


// IMMCNotificationClient methods

inline STDMETHODIMP_(HRESULT __stdcall) CNotificationClient::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId)
{
	// Only handle render (output) devices with console (default) role
    if (flow != eRender || role != eConsole)
        return S_OK;

    // Default audio device has been changed.
    ALDeviceDesc deviceDesc = SoundRenderA->pDeviceList->GetDeviceDesc(snd_device_id);

    xr_string DeviceName = deviceDesc.name;
    if (DeviceName == "Default Device")
    {
        SoundRenderA->bReady = false;
        SoundRenderA->pause_emitters(true);

        ALCdevice* OldDevice = SoundRenderA->pDevice;
        ALCcontext* OldContect = SoundRenderA->pContext;

        for (u32 it = 0; it < SoundRenderA->s_targets.size(); it++)
        {
            CSoundRender_TargetA* AlTarget = (CSoundRender_TargetA*)SoundRenderA->s_targets[it];
            AlTarget->_destroy();
        }

        SoundRenderA->DestroyEffect();
        SoundRenderA->pDevice = alcOpenDevice(deviceDesc.name_al);
        SoundRenderA->pContext = alcCreateContext(SoundRenderA->pDevice, nullptr);

        alcMakeContextCurrent(SoundRenderA->pContext);
        alcDestroyContext(OldContect);

        for (u32 it = 0; it < SoundRenderA->s_targets.size(); it++)
        {
            CSoundRender_TargetA* AlTarget = (CSoundRender_TargetA*)SoundRenderA->s_targets[it];
            AlTarget->_initialize();
        }

        SoundRenderA->LoadEffect();
        SoundRenderA->restart_emitters();
        SoundRenderA->pause_emitters(false);
        SoundRenderA->bReady = true;
    }

    return S_OK;
}

inline STDMETHODIMP_(HRESULT __stdcall) CNotificationClient::OnDeviceRemoved(LPCWSTR pwstrDeviceId)
{
    // An audio device has been removed.
    return S_OK;
}

inline STDMETHODIMP_(HRESULT __stdcall) CNotificationClient::OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) 
{
    // The state of an audio device has changed.
    return S_OK;
}

inline STDMETHODIMP_(HRESULT __stdcall) CNotificationClient::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) 
{
    // A property value of an audio device has changed.
    return S_OK;
}
