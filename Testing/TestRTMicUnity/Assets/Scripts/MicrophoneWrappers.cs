using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System;

public class MicrophoneWrappers 
{
	[AttributeUsage (AttributeTargets.Method)]
	public sealed class MonoPInvokeCallbackAttribute : Attribute {
		public MonoPInvokeCallbackAttribute (Type t) {}
	}

	[DllImport ("__Internal")]
	private static extern void SetMicAccessCallback(microphoneAccessDelegate callback);

	[DllImport ("__Internal")]
	private static extern void CheckMicAccess();

	static MicrophoneWrappers()
	{
		SetMicAccessCallback(NativeCallback);
	}

	public delegate void microphoneAccessDelegate( bool status );
	static Action<bool> MicrophoneAccessCallback;
	
	public static void RequestMicrophoneAccess(Action<bool> Callback)
	{
		MicrophoneAccessCallback = Callback;

		//This will access native functions to determine microphone access.
		//Then the marshalled callback delegate will be called with status.
		CheckMicAccess();
	}

	[MonoPInvokeCallback(typeof(microphoneAccessDelegate))]
	public static void NativeCallback(bool status)
	{
		if (MicrophoneAccessCallback != null)
			MicrophoneAccessCallback(status);
	}
}
