using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

public interface IMicrophoneDevice
{
    void AddHandlerToBufferChanged(Action<PhonemeBuffer.InternalBuffer> handler);
    void AddHandlerToBufferFinished(Action<PhonemeBuffer.InternalBuffer> handler);
    bool IsMicrophoneAvailible();
    void InitializeRecording(string calibrationFile);
    void StartRecording(string filePath);
    void StopRecording();
    void DestroyRecording();
    void DestroyDevice();
    bool ProcessRecording(string filePath);
    void Update();
    void SetCalibrationData(JObject calibrationData);
    JObject GetCalibrationData();
}
