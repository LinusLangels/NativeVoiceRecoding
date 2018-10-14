#define DEBUG_MODE

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.IO;
using System.Linq;

public class RecordRecordingMachine : RecordingMachineBase
{
	private bool isRecording;
	private float recordingTimer;

	public RecordRecordingMachine(float recordLength) : base (recordLength)
	{
		this.StateName = RecordingState.Recording;
		this.recordingTimer = 0f;
	}

	public override void EnterState()
	{
		this.isRecording = true;
		StartRecordingVoiceFromMic();

		#if DEBUG_MODE
		//start voice recording.
		Debug.Log ("Entered: " + this.StateName.ToString ());
		#endif
	}

	public override void ExitState()
	{
		if (this.isRecording)
		{
			StopRecordingVoiceFromMic();
			this.isRecording = false;
		}

		#if DEBUG_MODE
		//stop voice recording.
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

		//Are we done recording.
		bool isDonePlaying = this.recordingTimer >= this.recordLength ? true : false;

        //Update microphone device each frame.
        Device.Update();

		if (isDonePlaying)
		{
			RecordingMachineBase endRecord = new EndRecordingMachine (this.recordLength);
			endRecord.SetStartCallback(this.StartRecording);
			endRecord.SetStopCallback(this.StopRecording);

			//Record audio for a while.
			//when we are done we return a new endrecording machine to handler.
			return endRecord;
		}

		//Increment timer to check for done.
		this.recordingTimer += Time.deltaTime;

		return null;
	}

	public override void Abort(AbortMessage abortMessage, bool forceAbort)
	{
		this.AbortMessage = abortMessage;
		this.abortRecording = true;

		//Force cleanup of this state immediately.
		if (forceAbort)
		{
			ExitState();
		}
	}

	#region internal workings of the machine
	void StartRecordingVoiceFromMic()
	{
		if (this.recordLength < MAX_RECORD_LENGTH)
		{
			Device.StartRecording(WaveFilePath);
		}
		else
		{
			Abort(new AbortMessage("Recording is too long. Keep it below "+MAX_RECORD_LENGTH+" seconds.", true, true), false);
		}
	}

	void StopRecordingVoiceFromMic()
	{
		Device.StopRecording ();

		//Tear down any resources.
		Device.DestroyRecording ();
	}
	#endregion
}
