//
//  CalibrateAudioData.h
//  MicrophoneRecorder
//
//  Created by John Philipsson on 2016-01-14.
//  Copyright © 2016 John Philipsson. All rights reserved.
//

#ifndef CalibrateAudioData_h
#define CalibrateAudioData_h

#include <stdio.h>
#include <math.h>

typedef struct CALIBRATION_st {
    
    
    float maxAbsMean;
    float currentCalibrationScaleFactor;
    
    
}CALIBRATION_t;

void CalibrateAudio(CALIBRATION_t *calibrate, float *buffer, int avgMeanSize, int lengthOfBuffer);
float CalibrationEstimation(float *buffer, int avgMeanSize, int lengthOfBuffer);

#endif /* CalibrateAudioData_h */
