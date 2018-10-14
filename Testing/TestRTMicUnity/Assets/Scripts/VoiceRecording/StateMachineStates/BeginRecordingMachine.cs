#define DEBUG_MODE

using UnityEngine;
using System.Collections;
using System.IO;

public class BeginRecordingMachine : RecordingMachineBase
{
	public BeginRecordingMachine(float recordLength) : base (recordLength)
	{
		this.StateName = RecordingState.BeginRecording;
	}

	public override void EnterState()
	{
		if (this.StartRecording != null)
			this.StartRecording();

		Device.InitializeRecording (CalibrationFilePath);

		#if DEBUG_MODE
		//Make sure mic is ready and availible.
		Debug.Log ("Entered: " + this.StateName.ToString ());
		#endif
	}

	public override void ExitState()
	{
		#if DEBUG_MODE
		Debug.Log ("Exit: " + this.StateName.ToString ());
		#endif
	}

	public override RecordingMachineBase HandleState()
	{
		
		//Transition to recording state. This is where we actually use the mic.
		RecordingMachineBase record = new RecordRecordingMachine (this.recordLength);
		record.SetStartCallback(this.StartRecording);
		record.SetStopCallback(this.StopRecording);




		return record;
	}

	public override void Abort(AbortMessage abortMessage, bool forceAbort)
	{
		this.AbortMessage = abortMessage;
	}
}
