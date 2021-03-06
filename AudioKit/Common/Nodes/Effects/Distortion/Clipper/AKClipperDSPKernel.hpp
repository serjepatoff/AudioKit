//
//  AKClipperDSPKernel.hpp
//  AudioKit
//
//  Created by Aurelius Prochazka, revision history on Github.
//  Copyright (c) 2016 Aurelius Prochazka. All rights reserved.
//

#ifndef AKClipperDSPKernel_hpp
#define AKClipperDSPKernel_hpp

#import "DSPKernel.hpp"
#import "ParameterRamper.hpp"

#import <AudioKit/AudioKit-Swift.h>

extern "C" {
#include "soundpipe.h"
}

enum {
    limitAddress = 0
};

class AKClipperDSPKernel : public DSPKernel {
public:
    // MARK: Member Functions

    AKClipperDSPKernel() {}

    void init(int channelCount, double inSampleRate) {
        channels = channelCount;

        sampleRate = float(inSampleRate);

        sp_create(&sp);
        sp->sr = sampleRate;
        sp->nchan = channels;
        sp_clip_create(&clip);
        sp_clip_init(sp, clip);
        clip->lim = 1.0;

        limitRamper.init();
    }

    void start() {
        started = true;
    }

    void stop() {
        started = false;
    }

    void destroy() {
        sp_clip_destroy(&clip);
        sp_destroy(&sp);
    }

    void reset() {
        resetted = true;
        limitRamper.reset();
    }

    void setLimit(float value) {
        limit = clamp(value, 0.0f, 1.0f);
        limitRamper.setImmediate(limit);
    }


    void setParameter(AUParameterAddress address, AUValue value) {
        switch (address) {
            case limitAddress:
                limitRamper.setUIValue(clamp(value, 0.0f, 1.0f));
                break;

        }
    }

    AUValue getParameter(AUParameterAddress address) {
        switch (address) {
            case limitAddress:
                return limitRamper.getUIValue();

            default: return 0.0f;
        }
    }

    void startRamp(AUParameterAddress address, AUValue value, AUAudioFrameCount duration) override {
        switch (address) {
            case limitAddress:
                limitRamper.startRamp(clamp(value, 0.0f, 1.0f), duration);
                break;

        }
    }

    void setBuffers(AudioBufferList *inBufferList, AudioBufferList *outBufferList) {
        inBufferListPtr = inBufferList;
        outBufferListPtr = outBufferList;
    }

    void process(AUAudioFrameCount frameCount, AUAudioFrameCount bufferOffset) override {

        for (int frameIndex = 0; frameIndex < frameCount; ++frameIndex) {

            int frameOffset = int(frameIndex + bufferOffset);

            limit = limitRamper.getAndStep();
            clip->lim = (float)limit;

            for (int channel = 0; channel < channels; ++channel) {
                float *in  = (float *)inBufferListPtr->mBuffers[channel].mData  + frameOffset;
                float *out = (float *)outBufferListPtr->mBuffers[channel].mData + frameOffset;

                if (started) {
                    sp_clip_compute(sp, clip, in, out);
                } else {
                    *out = *in;
                }
            }
        }
    }

    // MARK: Member Variables

private:
    int channels = AKSettings.numberOfChannels;
    float sampleRate = AKSettings.sampleRate;

    AudioBufferList *inBufferListPtr = nullptr;
    AudioBufferList *outBufferListPtr = nullptr;

    sp_data *sp;
    sp_clip *clip;

    float limit = 1.0;

public:
    bool started = true;
    bool resetted = false;
    ParameterRamper limitRamper = 1.0;
};

#endif /* AKClipperDSPKernel_hpp */
