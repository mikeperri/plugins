#include <jni.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>

#include <stdint.h>
#include <vector>
#include <math.h>

#include <oboe/Oboe.h>
// #include <android_fopen.h>
#define TSF_IMPLEMENTATION
#include <tsf.h>

#include "models.h"

class AndroidTsfPlayer: public oboe::AudioStreamCallback {
public:
    AndroidTsfPlayer() {
        oboe::AudioStreamBuilder builder;
        // The builder set methods can be chained for convenience.
        builder.setSharingMode(oboe::SharingMode::Shared)
          ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
          ->setChannelCount(kChannelCount)
          ->setSampleRate(kSampleRate)
          ->setFormat(oboe::AudioFormat::Float)
          ->setCallback(this)
          ->openManagedStream(outStream);
    }

    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *oboeStream, void *audioData, int32_t numFrames) override {
        // __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Starting audio callback");
        float *floatData = (float *) audioData;

        int64_t framesRendered = 0;

        if (!mIsNextEventReady) {
            getIsNextEventReady();
        }

        while (mIsNextEventReady) {
            int64_t framesUntilEvent = mNextEvent.frame - mCurrentFrame + framesRendered;
            if (framesUntilEvent <= 0) {
                framesUntilEvent = 0;
            } else {
                // __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Rendering frames until event: %lli", framesUntilEvent);
                tsf_render_float(mTsf, floatData + framesRendered, mNextEvent.frame - (mCurrentFrame + framesRendered), 0);
            }

            if (mNextEvent.type == NoteOn) {
                // __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Note on");
                tsf_note_on(mTsf, mNextEvent.patchIndex, mNextEvent.pitch, mNextEvent.velocity);

            } else if (mNextEvent.type == NoteOff) {
                // __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Note off");
                tsf_note_off(mTsf, mNextEvent.patchIndex, mNextEvent.pitch);
            }

            framesRendered += framesUntilEvent;
            getIsNextEventReady();

            bool hasNextEvent = mIsNextEventReady;
            // __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "hasNextEvent: %d", hasNextEvent);
        }

        if (framesRendered < numFrames) {
            int64_t currentFrame = mCurrentFrame;
            // __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "About to render, mCurrentFrame: %lld, framesRendered: %lld, numFrames: %ld", currentFrame, framesRendered, numFrames);
            tsf_render_float(mTsf, floatData + framesRendered, numFrames - framesRendered, 0);
        }

        mCurrentFrame += numFrames;


        if (floatData[0] != 0.0 && floatData[1] != 0.0 && floatData[2] != 0.0 && floatData[3] != 0.0) {
            // __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Audio ready, first four samples: %f %f %f %f", floatData[0], floatData[1], floatData[2], floatData[3]);
        }
        return oboe::DataCallbackResult::Continue;
    }

    void loadSoundfont(const char* filename) {
        __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Starting load sound font, filename %s", filename);
        mTsf = tsf_load_filename(filename);
        __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Called tsf_load_filename, %i", mTsf);
        tsf_set_output(mTsf, TSF_STEREO_INTERLEAVED, kSampleRate, 0);
        __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Loaded sound font %s", filename);

        outStream->requestStart();
    }

    void noteOn(uint8_t patchIndex, uint8_t pitch, int64_t delayMs, float velocity) {
        // tsf_note_on(mTsf, patchIndex, pitch, velocity);

        ScheduledEvent event;
        event.frame = mCurrentFrame + millisToFrames(delayMs);
        event.type = NoteOn;
        event.patchIndex = patchIndex;
        event.pitch = pitch;
        event.velocity = velocity;

        // __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Scheduled note on at %lli", event.frame);

        scheduleEvent(event);
    }

    void noteOff(uint8_t patchIndex, uint8_t pitch, int64_t delayMs) {
        // tsf_note_off(mTsf, patchIndex, pitch);

        ScheduledEvent event;
        event.frame = mCurrentFrame + millisToFrames(delayMs);
        event.type = NoteOff;
        event.patchIndex = patchIndex;
        event.pitch = pitch;

        // __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Scheduled note off at %lli", event.frame);

        scheduleEvent(event);
    }

private:
    oboe::ManagedStream outStream;
    // Stream params
    static int constexpr kChannelCount = 2;
    static int constexpr kSampleRate = 44100;
    std::atomic<int64_t> mCurrentFrame { 0 };

    tsf* mTsf;
    std::vector<ScheduledEvent> mScheduledEvents;
    std::mutex mScheduledEventsMutex;
    ScheduledEvent mNextEvent;
    std::atomic<bool> mIsNextEventReady { false };

    int64_t millisToFrames(int64_t millis) {
        return (millis / 1000.0) * kSampleRate;
    }

    void scheduleEvent(ScheduledEvent event) {
        int i = 0;

        while (i < mScheduledEvents.size() && mScheduledEvents.at(i).frame < event.frame) {
            i++;
        }

        std::lock_guard<std::mutex> lock(mScheduledEventsMutex);
        mScheduledEvents.insert(mScheduledEvents.begin() + i, event);
    }

    void getIsNextEventReady() {
        std::lock_guard<std::mutex> lock(mScheduledEventsMutex);

        if (mScheduledEvents.size() && mScheduledEvents.at(0).frame < mCurrentFrame) {
            mNextEvent = mScheduledEvents.at(0);
            mScheduledEvents.erase(mScheduledEvents.begin());
            mIsNextEventReady = true;
        } else {
            mIsNextEventReady = false;
        }
    }
};


extern "C" {
    std::unique_ptr<AndroidTsfPlayer> player;
    tsf* TinySoundFont;

    // JNIEXPORT void JNICALL
    // Java_com_michaeljperri_flutter_1low_1latency_1audio_FlutterLowLatencyAudioPlugin_native_1onStart(JNIEnv *env, jobject instance,
    //                                                                      jobject jAssetManager) {

    //     AAssetManager *assetManager = AAssetManager_fromJava(env, jAssetManager);
    //     if (assetManager == nullptr) {
    //         __android_log_print(ANDROID_LOG_ERROR, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Could not load assetManager");
    //         return;
    //     }
    //     android_fopen_set_asset_manager(assetManager);
    //     __android_log_print(ANDROID_LOG_INFO, "FLUTTER_LOW_LATENCY_AUDIO_PLUGIN", "Successfully set asset manager for android_fopen");
    // }

    __attribute__((visibility("default"))) __attribute__((used))
    int32_t prepareMidi(const char* filename) {
        player = std::make_unique<AndroidTsfPlayer>();
        player->loadSoundfont(filename);

        return 0;
    }

    __attribute__((visibility("default"))) __attribute__((used))
    int32_t noteOn(uint8_t patchIndex, uint8_t pitch, int64_t delayMs, float velocity) {
        player->noteOn(patchIndex, pitch, delayMs, velocity);

        return 0;
    }

    __attribute__((visibility("default"))) __attribute__((used))
    int32_t noteOff(uint8_t patchIndex, uint8_t pitch, int64_t delayMs) {
        player->noteOff(patchIndex, pitch, delayMs);

        return 0;
    }
}
