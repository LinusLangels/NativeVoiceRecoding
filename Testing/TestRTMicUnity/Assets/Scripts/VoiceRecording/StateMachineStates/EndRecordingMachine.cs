#define DEBUG_MODE

using UnityEngine;
using System.IO;
using System;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

public class EndRecordingMachine : RecordingMachineBase
{
	public EndRecordingMachine(float recordLength) : base (recordLength)
	{
		this.StateName = RecordingState.EndRecording;
	}

	public override void EnterState()
	{
		//Do post processing on the recorded audio. And make sure something was recorded. (check for silence)
		bool status = Device.ProcessRecording(WaveFilePath);

		if (status)
		{
			//This will terminate the state machine loop.
			SetEndState();

            JObject calibrationData = Device.GetCalibrationData();

            //This recording produced calibration data.
            if (calibrationData != null)
            {
                try
                {
                    if (File.Exists(CALIBRATION_FILE_PATH))
                    {
                        File.Delete(CALIBRATION_FILE_PATH);
                    }

                    File.WriteAllText(CALIBRATION_FILE_PATH, calibrationData.ToString());
                }
                catch (Exception e)
                {
                    Debug.LogError("Unable to write microphone calibration data: " + e.Message);
                }

                Debug.Log("Saved calibration data");
                Device.SetCalibrationData(null);
            }
		}
		else
		{
			if (File.Exists(WaveFilePath))
			{
				try
				{
					//File.Delete(WaveFilePath);
				}
				catch (System.Exception e)
				{
					Debug.LogError("Unable to delete silent recording: " + e.Message);
				}
			}

			Abort(new AbortMessage("No sound was heard, check microphone.", true, true), false);
		}

		#if DEBUG_MODE
		Debug.Log ("Entered: " + this.StateName.ToString ());
		#endif
	}
	
	public override void ExitState()
	{
		if (this.StopRecording != null)
			this.StopRecording();

		#if DEBUG_MODE
		Debug.Log ("Exit: " + this.StateName.ToString ());
		#endif
	}

	public override RecordingMachineBase HandleState()
	{
		if (this.abortRecording)
		{
			RecordingMachineBase invalid = new InvalidRecordingMachine (this.AbortMessage);
			invalid.SetStartCallback(this.StartRecording);
			invalid.SetStopCallback(this.StopRecording);

			return invalid;
		}

		return null;
	}

	public override void Abort(AbortMessage abortMessage, bool forceAbort)
	{
		this.AbortMessage = abortMessage;
		this.abortRecording = true;
	}
}
