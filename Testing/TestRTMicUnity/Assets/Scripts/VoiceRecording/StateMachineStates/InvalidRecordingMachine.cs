//#define DEBUG_MODE

using UnityEngine;
using System.Collections;

public class InvalidRecordingMachine : RecordingMachineBase
{
	public InvalidRecordingMachine(AbortMessage abortMessage) : base ()
	{
		this.AbortMessage = abortMessage;
		this.StateName = RecordingState.InvalidRecording;
	}

	public override void EnterState()
	{
		//This will terminate the state machine loop.
		SetEndState();

		#if DEBUG_MODE
		//Make sure mic is closed. Anything else?
		//Make sure we actually recorded something.
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
		//This is a terminal state.
		return null;
	}

	public override void Abort(AbortMessage abortMessage, bool forceAbort)
	{
		this.AbortMessage = abortMessage;
	}
}
