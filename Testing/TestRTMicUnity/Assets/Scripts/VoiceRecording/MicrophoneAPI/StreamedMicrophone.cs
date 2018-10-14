#define DEBUG_MODE

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using System.IO;
using Plotagon.Serialization;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

public class StreamedMicrophone : IMicrophoneDevice
{
	[AttributeUsage (AttributeTargets.Method)]
	public sealed class MonoPInvokeCallbackAttribute : Attribute {
		public MonoPInvokeCallbackAttribute (Type t) {}
	}

	public delegate void stringParameterCallback(string str);
	public delegate void phonemeParameterCallback(IntPtr phonemeDataPointer);
    public delegate void calibrationParametersCallback(float maxABSMean, float scaleFactor);

    #if UNITY_IOS && !UNITY_EDITOR
	[DllImport ("__Internal")]
    #else
    [DllImport ("MicrophoneRecorder")]
	#endif
	private static extern void SetDebugLog(stringParameterCallback functionDelegate);

	#if UNITY_IOS && !UNITY_EDITOR
	[DllImport ("__Internal")]
	#else
	[DllImport ("MicrophoneRecorder")]
	#endif
    private static extern void SetPhonemeCallback(phonemeParameterCallback functionDelegate);

    #if UNITY_IOS && !UNITY_EDITOR
	[DllImport ("__Internal")]
    #else
    [DllImport("MicrophoneRecorder")]
    #endif
    private static extern void SetCalibrationCallback(calibrationParametersCallback functionDelegate);

    #if UNITY_IOS && !UNITY_EDITOR
	[DllImport ("__Internal")]
    #else
    [DllImport ("MicrophoneRecorder")]
	#endif
	private static extern void DestroyCallbacks();

	#if UNITY_IOS && !UNITY_EDITOR
	[DllImport ("__Internal")]
	#else
	[DllImport ("MicrophoneRecorder")]
	#endif
    private static extern bool Bridge_IsMicrophoneAvailible();

	#if UNITY_IOS && !UNITY_EDITOR
	[DllImport ("__Internal")]
	#else
	[DllImport ("MicrophoneRecorder")]
	#endif
	private static extern void Bridge_InitRecording();
	
	#if UNITY_IOS && !UNITY_EDITOR
	[DllImport ("__Internal")]
	#else
	[DllImport ("MicrophoneRecorder")]
#endif
    private static extern void Bridge_StartRecording(int complexity, string path, int algoNumber, float calibrationFactor, bool calibrate);

#if UNITY_IOS && !UNITY_EDITOR
	[DllImport ("__Internal")]
#else
    [DllImport ("MicrophoneRecorder")]
	#endif
	private static extern void Bridge_StopRecording(int algoNumber);

	#if UNITY_IOS && !UNITY_EDITOR
	[DllImport ("__Internal")]
	#else
	[DllImport ("MicrophoneRecorder")]
	#endif
	private static extern void Bridge_Destroy();

	#if UNITY_IOS && !UNITY_EDITOR
	[DllImport ("__Internal")]
	#else
	[DllImport ("MicrophoneRecorder")]
	#endif
	private static extern void Bridge_TrimRecording(string filePath, float startTime, float stopTime);

	static StreamedMicrophone Instance;

	private PhonemeBuffer phonemeBuffer;
    private Queue<PhonemeContainer> eventQueue;
    private System.Object locker;
    private string calibrationFile;
    private float maxABSMean;
    private float scaleFactor;
    private JObject calibrationData;

    public StreamedMicrophone()
    {
        Instance = this;

        this.locker = new System.Object();
        this.eventQueue = new Queue<PhonemeContainer>();
        this.maxABSMean = 1.0f;
        this.scaleFactor = 1.0f;

        SetDebugLog (DebugLogCallback);
		SetPhonemeCallback (PhonemeCallback);
        SetCalibrationCallback(CalibrationCallback);
    }

    public bool IsMicrophoneAvailible()
    {
        bool availible = Bridge_IsMicrophoneAvailible();

        return availible;
    }

    public void InitializeRecording(string calibrationFile)
    {
        //Tweak buffer length to something that looks good.
        //Lower buffer lengths requires more temporal buffers so we get smooth lipsync data.
        this.phonemeBuffer = new PhonemeBuffer(0.1f, 4);
        this.calibrationFile = calibrationFile;
        this.calibrationData = null;

        if (File.Exists(this.calibrationFile))
        {
            JObject calibrationData = JObject.Parse(File.ReadAllText(this.calibrationFile));
            this.maxABSMean = calibrationData.Value<float>("maxABSMean");
            this.scaleFactor = calibrationData.Value<float>("scaleFactor");
        }

        Bridge_InitRecording();
    }

    float GetDefaultScaleFactor()
    {
        return this.scaleFactor;
    }

    public void StartRecording(string filePath)
	{
		//For safety.
		if (File.Exists(filePath))
		{
			File.Delete(filePath);
		}

        if (File.Exists(this.calibrationFile))
        {
            Bridge_StartRecording(0, filePath, 0, this.scaleFactor, false);
        }
        else
        {
            //If no calibration file exists we get default scale factor for the platform and generate a calibration file.
            Bridge_StartRecording(0, filePath, 0, GetDefaultScaleFactor(), true);
        }
    }

	public void StopRecording()
	{
		Bridge_StopRecording (0);
	}

	public void DestroyRecording()
	{
		Bridge_Destroy ();
	}

	public bool ProcessRecording(string filePath)
	{
		//This will invoke finalize handler in the phonemebuffer instance, this will inturn notify animation system.
		PhonemeBuffer.InternalBuffer completeBuffer = this.phonemeBuffer.Finalize ();

		//Empty buffer means no sound was recorded, return false.
		if (completeBuffer.NumberOfItems () == 0) 
		{
			return false;
		}

		//Trim audio file data.
		Bridge_TrimRecording(filePath, 0f, completeBuffer.GetBufferLength());

		//Make this buffer eligable for GC.
		this.phonemeBuffer = null;

		return true;
	}

	public void DestroyDevice()
	{
        DestroyCallbacks();
	}

	public void AddHandlerToBufferChanged(Action<PhonemeBuffer.InternalBuffer> handler)
	{
		if (this.phonemeBuffer != null) 
		{
			this.phonemeBuffer.SubscribeBuffersChanged (handler);
		}
	}

	public void AddHandlerToBufferFinished(Action<PhonemeBuffer.InternalBuffer> handler)
	{
		if (this.phonemeBuffer != null) 
		{
			this.phonemeBuffer.SubscribeOnFinished (handler);
		}
	}

	public void AddPhonemeToBuffer(PhonemeContainer phoneme)
	{
        lock(this.locker)
        {
            //Make sure these always get processed on main thread.
            if (this.eventQueue != null)
            {
                this.eventQueue.Enqueue(phoneme);
            }
        }
	}

    public void SetCalibrationData(JObject calibrationData)
    {
        this.calibrationData = calibrationData;
    }

    public JObject GetCalibrationData()
    {
        return this.calibrationData;
    }

    public void Update()
    {
        if (this.eventQueue == null)
            return;

        while (this.eventQueue.Count > 0)
        {
            PhonemeContainer phoneme = this.eventQueue.Dequeue();

            if (this.phonemeBuffer != null)
            {
                this.phonemeBuffer.AddPhoneme(phoneme);
            }
        }
    }
	
	[MonoPInvokeCallback(typeof(phonemeParameterCallback))]
	public static void PhonemeCallback(IntPtr phonemeDataPointer)
	{
		if (phonemeDataPointer != IntPtr.Zero)
		{
			PhonemeContainer phoneme = (PhonemeContainer)Marshal.PtrToStructure(phonemeDataPointer, typeof(PhonemeContainer));

			if (Instance != null)
			{
				//Debug.Log ("Phoneme is: " + phoneme.Phoneme);
				//Add last generated phoneme to our front buffer.
				Instance.AddPhonemeToBuffer(phoneme);
			}
		}
	}

    [MonoPInvokeCallback(typeof(calibrationParametersCallback))]
    public static void CalibrationCallback(float maxABSMean, float scaleFactor)
    {
        if (Instance != null)
        {
            JObject calibrationData = new JObject();
            calibrationData["maxABSMean"] = maxABSMean;
            calibrationData["scaleFactor"] = scaleFactor;

            Instance.SetCalibrationData(calibrationData);
        }
    }

    [MonoPInvokeCallback(typeof(stringParameterCallback))]
	public static void DebugLogCallback(string message)
	{
        #if DEBUG_MODE
        Debug.Log("Native Log: " + message);
        #endif
    }
}