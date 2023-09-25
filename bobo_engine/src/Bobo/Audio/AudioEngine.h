#pragma once

/// Copied Code!
/// Credit to: https://github.com/rosshoyt/FMOD-Audio-Engine/tree/master

/// 
/// @file AudioEngine.h
/// 
/// FMOD wrapper which loads sound files (.wav, .mp3, .ogg etc) and FMOD soundbanks (.bank files)
/// and supports looping or one-shot playback in stereo, as well as customizable 3D positional audio
/// Implements the FMOD Studio and FMOD Core API's to allow audio file-based implementations,
/// alongside/in addition to use of FMOD Studio Sound Banks.
///
/// @author Ross Hoyt
/// @dependencies FMOD Studio & Core
/// 

#include "bobopch.h"

#include "fmod.hpp"
#include "fmod_studio.hpp"
#include "fmod_errors.h"

#include <map>

#include "SoundInfo.h"

namespace Bobo {
    /**
     * Error Handling Function for FMOD Errors
     * @param result - the FMOD_RESULT generated during every FMOD
     */
    void CheckForFMODError(FMOD_RESULT result, const char* file, int line);
    #define ERRCHECK(_result) CheckForFMODError(_result, __FILE__, __LINE__)

    /**
     * Class that handles the process of loading and playing sounds by wrapping FMOD's functionality.
     * Deals with all FMOD calls so that FMOD-specific code does not need to be used outside this class.
     * Only one AudioEngine should be constructed for an application.
     */
    class BOBO_API AudioEngine {
        
    public:
        static AudioEngine* GetInstance()
        {
            static AudioEngine instance = AudioEngine();
            return &instance;
        }

        /**
        * Initializes Audio Engine Studio and Core systems
        * FMOD's Distance factor is set to 1.0f by default (1 meter/ 3D game unit)
        */
        void Init();

        // Method which should be called every frame of the game loop
        void Update();

        /**
        * Loads a mono or stereo audio file, which will playback in tradidional stereo mode (IE, not positional audio)
        * The file is read into the cache to prepare for later playback.
        * Only reads file and creates the sound if it has not already been added to the cache.
        * To play the sound later, use method playSound()
        */
        void LoadSound(SoundInfo soundInfo);

        /**
        * Plays a sound file using FMOD's low level audio system. If the sound file has not been
        * previously loaded using loadSoundFile(), a console message is displayed
        *
        * @var filename - relative path to file from project directory. (Can be .OGG, .WAV, .MP3,
        *                 or any other FMOD-supported audio format)
        */
        void PlaySound(SoundInfo soundInfo);

        // Stops a Sound
        void StopSound(SoundInfo soundInfo);

        /**
        * Updates the position of a looping 3D sound that has already been loaded and is playing back.
        * The SoundInfo object's position coordinates will be used for the new sound position, so
        * SoundInfo::set3DCoords(x,y,z) should be called before this method to set the new desired location.
        */
        void Update3DSoundPosition(SoundInfo soundInfo);

        // Returns whether or not a sound is playing
        bool SoundIsPlaying(SoundInfo soundInfo);

        /**
        * Sets the position of the listener in the 3D scene.
        * @var posX, posY, posZ - 3D translation of listener
        * @var forwardX, forwardY, forwardZ - forward angle character is looking in
        * @var upX, upY, upZ - up which must be perpendicular to forward vector
        */
        void Set3DListenerPosition(float posX, float posY, float posZ,
           float forwardX, float forwardY, float forwardZ,
           float upX, float upY, float upZ);

    private:
        // FMOD Studio API system, which can play FMOD sound banks (*.bank)
        FMOD::Studio::System* p_StudioSystem = nullptr;

        // FMOD's low-level audio system which plays audio files and is obtained from Studio System
        FMOD::System* p_LowLevelSystem = nullptr;

        /*
        * Map which caches FMOD Low-Level sounds
        * Key is the SoundInfo's uniqueKey field.
        * Value is the FMOD::Sound* to be played back.
        * TODO Refactor to use numeric UID as key
        */
        std::map<std::string, FMOD::Sound*> m_Sounds;

        /*
        * Map which stores the current playback channels of any playing sound loop
        * Key is the SoundInfo's uniqueKey field.
        * Value is the FMOD::Channel* the FMOD::Sound* is playing back on.
        */
        std::map<std::string, FMOD::Channel*> m_LoopsPlaying;

        // Map which stores the soundbanks loaded with loadFMODStudioBank()        
        std::map<std::string, FMOD::Studio::Bank*> m_SoundBanks;

        // Max FMOD::Channels for the audio engine 
        static const unsigned int MAX_AUDIO_CHANNELS = 1024;
        // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.
        const float DISTANCEFACTOR = 1.0f;
        // Listener position, initialized to default value
        FMOD_VECTOR m_ListenerPos = { 0.0f, 0.0f, -1.0f * DISTANCEFACTOR };
        // Listener forward vector, initialized to default value
        FMOD_VECTOR m_Forward = { 0.0f, 0.0f, 1.0f };
        // Listener upwards vector, initialized to default value
        FMOD_VECTOR m_Up = { 0.0f, 1.0f, 0.0f };

        AudioEngine();

        // Checks if a sound file is in the soundCache
        bool SoundLoaded(SoundInfo soundInfo);

        // Sets the 3D position of a sound        
        void Set3dChannelPosition(SoundInfo soundInfo, FMOD::Channel* channel);
    };
}