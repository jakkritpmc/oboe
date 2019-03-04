/*
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include "oboe/Definitions.h"
#include "SawPingGenerator.h"

using namespace flowgraph;

SawPingGenerator::SawPingGenerator()
        : OscillatorBase()
        , mRequestCount(0)
        , mAcknowledgeCount(0)
        , mLevel(0.0f) {
}

SawPingGenerator::~SawPingGenerator() { }

int32_t SawPingGenerator::onProcess(
        int64_t framePosition,
        int numFrames) {

    frequency.pullData(framePosition, numFrames);
    amplitude.pullData(framePosition, numFrames);

    const float *frequencies = frequency.getBlock();
    const float *amplitudes = amplitude.getBlock();
    float *buffer = output.getBlock();

    if (mRequestCount.load() > mAcknowledgeCount.load()) {
        mPhase = -1.0f;
        mLevel = 1.0;
        mAcknowledgeCount++;
    }

    // Check level to prevent numeric underflow.
    if (mLevel > 0.000001) {
        for (int i = 0; i < numFrames; i++) {
            float sawtooth = incrementPhase(frequencies[i]);
            *buffer++ = (float) (sawtooth * mLevel * amplitudes[i]);
            mLevel *= 0.999;
        }
    } else {
        for (int i = 0; i < numFrames; i++) {
            *buffer++ = 0.0f;
        }
    }

    return numFrames;
}

void SawPingGenerator::setEnabled(bool enabled) {
    if (enabled) {
        mRequestCount++;
    }
}

