using UnityEngine;
using System.Collections;
using System.IO;
using System;

public enum RecordingState 
{ 
	InvalidRecording,
	BeginRecording,
	Recording,
	EndRecording,
}

public abstract class RecordingMachineBase
{
	public static readonly float RECORD_PADDING = 0.5f;
	public static readonly float MAX_RECORD_LENGTH = 20f;
    //Path to calibration file.
    public static readonly string CALIBRATION_FILE_PATH = Application.persistentDataPath + "/calibration.json";

    protected static IMicrophoneDevice Device;
	protected static string WaveFilePath = string.Empty;
    protected static string CalibrationFilePath = string.Empty;

    public static void SetRecordingFilePath(string filePath)
	{
        string directoryName = Path.GetDirectoryName(filePath);

        if (!Directory.Exists(directoryName))
        {
            Directory.CreateDirectory(directoryName);
        }

		WaveFilePath = filePath;
	}

    public static void SetCalibrationFilePath(string filePath)
    {
        CalibrationFilePath = filePath;
    }

    public static void SetRecordingDevice(IMicrophoneDevice device)
	{
		Device = device;
	}

	public static string GetWavFilePath()
	{
		return WaveFilePath;
	}

	//Callbacks to be passed in the state chain.
	protected Action StartRecording;
	protected Action StopRecording;
	
	protected float recordLength;
	protected bool abortRecording;

	public RecordingState StateName { get; protected set; }
	public AbortMessage AbortMessage { get; protected set; }
	public bool IsEndState { get; protected set; }

	public RecordingMachineBase() { }
	public RecordingMachineBase(float recordLength)
	{
		this.recordLength = recordLength;
	}

	public void SetStartCallback(Action startDelegate)
	{
		this.StartRecording = startDelegate;
	}

	public void SetStopCallback(Action stopDelegate)
	{
		this.StopRecording = stopDelegate;
	}

	public void SetEndState()
	{
		this.IsEndState = true;
	}
	
	public abstract void EnterState();
	public abstract void ExitState();
	public abstract RecordingMachineBase HandleState();
	public abstract void Abort(AbortMessage abortMessage, bool forceAbort);
}

public struct AbortMessage
{
	public string Message { get; private set; }
	public bool Error { get; private set; }
	public bool DisplayError { get; private set; }

	public AbortMessage(string message, bool error, bool displayError)
	{
		this.Message = message;
		this.Error = error;
		this.DisplayError = displayError;
	}
}
