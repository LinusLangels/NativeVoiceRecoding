using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using Plotagon.Serialization;

public class Main : MonoBehaviour 
{
	private string text = "hello there, write longer dialogue here. to we can see.";
	private int syllablesPerMinute = 220;
	private bool isAborting = false;
    private bool isCalibrating = false;
	private VoiceRecordingAnimation voiceAnimation;
	private List<IAnimationChannel> lipsyncChannels;
	private FacialAnimationClip recordedClip;
	private RecordingMachineBase currentRecordingState;

	public GameObject Actor;
	public FacialAnimation animationTarget;
	public IMicrophoneDevice device;

	private AssetBundle animationsDatabase;

	void Start () 
	{
		Application.targetFrameRate = 30;

		this.device = new StreamedMicrophone();

        //Set microphone to native device implementation.
        RecordingMachineBase.SetRecordingDevice (device);

		#if UNITY_IOS && !UNITY_EDITOR
		this.animationsDatabase = AssetBundle.LoadFromFile(Application.streamingAssetsPath + "/FacialAnimations/iOS/FacialAnimationsDatabase.facialanimation");
		#else
		this.animationsDatabase = AssetBundle.LoadFromFile(Application.streamingAssetsPath + "/FacialAnimations/Standalone/FacialAnimationsDatabase.facialanimation");
		#endif

		if (this.animationsDatabase != null)
		{
			GameObject mainGO = this.animationsDatabase.mainAsset as GameObject;
			GameObject.Instantiate(mainGO);
		}

		this.Actor = GameObject.Find("deadtrigger2.buck");

		this.animationTarget = this.Actor.GetComponent<FacialAnimation>();

		if (this.animationTarget != null)
		{
			Transform headTransform = TransformHelp.FindChildRecursive(this.Actor.transform, "BaseHead");

			this.animationTarget.SetTargetMesh(headTransform.GetComponent<SkinnedMeshRenderer>());
		}

		#if UNITY_IOS && !UNITY_EDITOR
		MicrophoneWrappers.RequestMicrophoneAccess((bool access) => {
			Debug.Log("Mic access: " + access);
		});
		#endif

		AudioConfiguration config = AudioSettings.GetConfiguration();

		config.sampleRate = 44100;
		config.dspBufferSize = 512;
		config.speakerMode = AudioSpeakerMode.Mono;

		AudioSettings.Reset(config);
	}

	void OnDestroy()
	{
		if (currentRecordingState != null)
		{
			currentRecordingState.Abort(new AbortMessage("Engine stopped.", true, false), true);
		}

		if (device != null) {
			this.device.DestroyDevice();
		}

		if (this.animationsDatabase != null)
		{
			this.animationsDatabase.Unload(true);
		}
	}

	void OnGUI ()
	{
		GUIStyle style = new GUIStyle(GUI.skin.textField);
		style.fixedWidth = Screen.width;
		style.fixedHeight = 100;

		this.text = GUILayout.TextArea (this.text, style);

        if (!this.isCalibrating)
        {
            if (GUI.Button(new Rect(70, 100, 150, 100), "Record"))
            {
                this.isAborting = false;
                StartRecording();
            }

            if (GUI.Button(new Rect(70, 250, 150, 100), "Abort"))
            {
                this.isAborting = true;
            }
        }

        if (this.currentRecordingState != null && this.currentRecordingState.StateName == RecordingState.EndRecording) 
		{
			if (GUI.Button (new Rect (70, 350, 150, 100), "Play"))
			{ 
				string path =  RecordingMachineBase.GetWavFilePath();
				StartCoroutine(PlayRecordedAudio(path, this.lipsyncChannels));
			}
		}
	}

	void StartRecording()
	{
        bool availible = this.device.IsMicrophoneAvailible();

        //Make sure we have a microphone.
        if (!availible)
            return;

		//This will estimate for how long we are to record.
		float estimatedSpeakTime = TextAnalysisTools.EsimateTextLength (this.text, this.syllablesPerMinute) + RecordingMachineBase.RECORD_PADDING;

		UnityEngine.Debug.Log ("Speak Time: " + estimatedSpeakTime);

		//Clear states between each run.
		ClearRecordingState();

		RecordingMachineBase.SetRecordingFilePath(Application.persistentDataPath+ "/" + Time.realtimeSinceStartup + ".wav");
        RecordingMachineBase.SetCalibrationFilePath(Application.persistentDataPath + "/microphonecalibration.json");

        //Setup initial state where our record state will begin.
        RecordingMachineBase begin = new BeginRecordingMachine(estimatedSpeakTime);

		//Starting here.
		SetRecordingState (begin);

		FacialAnimationClip clip = this.animationTarget.ConstructFacialAnimationClip("dialogue", "neutral", "StandingNormal");
		//Make sure we call this after record state is setup.
		this.voiceAnimation = new VoiceRecordingAnimation(clip, this.animationTarget, estimatedSpeakTime, null);

		this.voiceAnimation.SubscribeOnFinished( (List<IAnimationChannel> lipsyncChannels, List<PhonemeContainer> phonemes, string phonemePath) => { 
			this.lipsyncChannels = lipsyncChannels;
		});

		//Couple callbacks from microphone to the animation component.
		this.device.AddHandlerToBufferChanged(voiceAnimation.UpdateAnimation);

		//Let animation component know when we are done.
		this.device.AddHandlerToBufferFinished(voiceAnimation.FinalizeAnimation);


		StartCoroutine (ProcessRecordingState ());
	}

	IEnumerator ProcessRecordingState()
	{
		while (this.currentRecordingState != null && !this.currentRecordingState.IsEndState) 
		{
			HandleRecordingState();

			//Abort recording. Cleanup.
			if (this.isAborting)
			{
				currentRecordingState.Abort(new AbortMessage("Recording stopped.", true, false), false);
			}

			yield return null;
		}

		this.currentRecordingState.ExitState ();

		//Cleanup the animation component.
		if (this.voiceAnimation != null)
			this.voiceAnimation.Destroy();
	}

	// voice recording
	IEnumerator PlayRecordedAudio(string path, List<IAnimationChannel> animationChannels)
	{
		if (File.Exists (path)) 
		{
			WWW audioLoader = new WWW ("file://" + path);

			while (!audioLoader.isDone)
				yield return null;

			//As frames are generated they are appended to this clip. (will lag behind by bufferLength * 30)
			this.recordedClip = this.animationTarget.ConstructFacialAnimationClip("dialogue", "neutral", "StandingNormal");

			foreach (IAnimationChannel channel in animationChannels)
			{
				this.recordedClip.AddAnimationChannel(channel);
			}

			this.animationTarget.PlayFacialAnimation(this.recordedClip, true, 1.0f, 0f);

			AudioClip clip = audioLoader.audioClip;
			AudioSource.PlayClipAtPoint(clip, Vector3.zero);

			/*
			float timer = 0f;

			while (timer < clip.length)
			{
				timer += Time.deltaTime;
				yield return null;
			}

			this.recordedClip.StopAnimation();
			*/
		}
	}
	
	public void ClearRecordingState()
	{
		this.currentRecordingState = null;
	}
	
	public void SetRecordingState(RecordingMachineBase newState)
	{
		if (this.currentRecordingState != null) 
		{
			this.currentRecordingState.ExitState();
		}
		
		this.currentRecordingState = newState;
		
		if (currentRecordingState != null)
		{
			currentRecordingState.EnterState();
		}
	}
	
	public void HandleRecordingState()
	{
		if (this.currentRecordingState == null)
			return;
		
		RecordingMachineBase newMachine = this.currentRecordingState.HandleState ();
		
		//If current state has changed we transition to the new state.
		if (newMachine != null)
		{
			SetRecordingState(newMachine);
		}
	}
}
