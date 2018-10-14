//#define DEBUG_MODE

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using Plotagon.Serialization;

#if UNITY_EDITOR
using Newtonsoft.Json.Linq;
using Newtonsoft.Json;
using UnityEditor;
#endif

public class VoiceRecordingAnimation
{
	private LipSyncData lipSyncer;
	private FacialAnimation animationTarget;
	private FacialAnimationClip streamedAnimationClip;
    private List<IAnimationChannel> streamedLipsyncChannels;
    private bool hasBegunAnimating;
	private Action<List<IAnimationChannel>, List<PhonemeContainer>, string> OnFinished;
	private int estimatedSpeakFrames;
    private bool hasStartedStreaming;
    private float startupTime;
    private float timeSinceStart;
    private string phonemesPath;

    #if UNITY_EDITOR
    private static EditorWindow visemeVisualizer;
	#endif

	public VoiceRecordingAnimation(FacialAnimationClip clip, FacialAnimation animationTarget, float estimatedSpeakTime, string phonemesPath)
	{
        this.streamedAnimationClip = clip;
		this.animationTarget = animationTarget;
        this.phonemesPath = phonemesPath;
		this.lipSyncer = new LipSyncData ();

		//Used to size our animation channels, this way the internal array in the keyframes list should be big enough.
		this.estimatedSpeakFrames = (int)(FacialAnimation.FRAMERATE * estimatedSpeakTime);

        //Create all our streamed lipsync channels.
        this.streamedLipsyncChannels = new List<IAnimationChannel>(7);
        this.streamedLipsyncChannels.Add(new RotationChannel("Mid_Head_Jnt_03", new List<Vector3>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        this.streamedLipsyncChannels.Add(new MorphChannel("Corner_In", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        this.streamedLipsyncChannels.Add(new MorphChannel("I", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        this.streamedLipsyncChannels.Add(new MorphChannel("Lip_LowerUp", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        this.streamedLipsyncChannels.Add(new MorphChannel("Lip_LowerDown", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        this.streamedLipsyncChannels.Add(new MorphChannel("Lip_UpperUp", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        this.streamedLipsyncChannels.Add(new MorphChannel("Lip_UpperDown", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));

		//Add our streamed channels. This will also remove any previous channel with the same name.
        for(int i = 0; i < this.streamedLipsyncChannels.Count; i++)
        {
            IAnimationChannel channel = this.streamedLipsyncChannels[i];
            this.streamedAnimationClip.AddAnimationChannel(channel);
        }

        //Stream data into these animation channels.
        this.lipSyncer.SetLipSyncChannels(this.streamedLipsyncChannels);

		this.hasBegunAnimating = false;
        this.hasStartedStreaming = false;

        #if UNITY_EDITOR && DEBUG_MODE
        //Close previous instance of window.
        if (visemeVisualizer != null)
            visemeVisualizer.Close();

        EditorApplication.ExecuteMenuItem("Plotagon/VisemeVisualizer");
        visemeVisualizer = EditorWindow.focusedWindow;

        //Allows us to see results in unitys editor window.
        for (int i = 0; i < this.streamedLipsyncChannels.Count; i++)
        {
            IAnimationChannel channel = this.streamedLipsyncChannels[i];
            channel.AddDebugWindow(visemeVisualizer);
        }
        #endif
    }

    public FacialAnimationClip GetFacialAnimationClip()
    {
        return this.streamedAnimationClip;
    }

	public void Destroy()
	{
		this.hasBegunAnimating = false;

		if (this.streamedAnimationClip != null)
		{
			this.streamedAnimationClip.StopAnimation();
		}
	}

	public void SubscribeOnFinished(Action<List<IAnimationChannel>, List<PhonemeContainer>, string> handler)
	{
		this.OnFinished = handler;
	}

	public void FinalizeAnimation(PhonemeBuffer.InternalBuffer completeBuffer)
	{
		//DebugBuffer(completeBuffer);

		//Contains all phonemes for entire recording.
		List<PhonemeContainer> phonemes = completeBuffer.GetBufferData();

        //Create all our streamed lipsync channels.
        List<IAnimationChannel> lipsyncChannels = new List<IAnimationChannel>(7);
        lipsyncChannels.Add(new RotationChannel("Mid_Head_Jnt_03", new List<Vector3>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        lipsyncChannels.Add(new MorphChannel("Corner_In", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        lipsyncChannels.Add(new MorphChannel("I", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        lipsyncChannels.Add(new MorphChannel("Lip_LowerUp", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        lipsyncChannels.Add(new MorphChannel("Lip_LowerDown", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        lipsyncChannels.Add(new MorphChannel("Lip_UpperUp", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));
        lipsyncChannels.Add(new MorphChannel("Lip_UpperDown", new List<float>(50), false, false, true, true, FacialAnimation.FRAMERATE));

        this.lipSyncer = new LipSyncData();

		//Stream data into these animation channels.
		this.lipSyncer.SetLipSyncChannels(lipsyncChannels);

		//Calculate lipsync data.
		this.lipSyncer.StreamedTextToSpeech(phonemes, 0f);

		if (this.OnFinished != null)
			this.OnFinished(lipsyncChannels, phonemes, this.phonemesPath);
	}

	public void UpdateAnimation(PhonemeBuffer.InternalBuffer frontBuffer)
	{
        if (!this.hasStartedStreaming)
        {
            this.hasStartedStreaming = true;
            this.startupTime = Time.realtimeSinceStartup;
        }

        //Keep track of time when animation data comes in, this allows us to track what frame we should be on.
        this.timeSinceStart = Time.realtimeSinceStartup - this.startupTime;

        float mergeBufferLength;
		List<PhonemeContainer> mergeBuffer = CreateMergeBuffer(frontBuffer, out mergeBufferLength);

		//Determine how much of the mergebuffer to keep after calculating animation.
		float offsetStream = mergeBufferLength - frontBuffer.GetBufferLength();

		this.lipSyncer.StreamedTextToSpeech(mergeBuffer, offsetStream);

		//Only triggered once on the first callback.
		if (!this.hasBegunAnimating)
		{
			this.animationTarget.PlayFacialAnimation(this.streamedAnimationClip, true, 0.5f, 0f);
			this.hasBegunAnimating = true;
		}

        #if UNITY_EDITOR && DEBUG_MODE
        for (int i = 0; i < this.streamedLipsyncChannels.Count; i++)
        {
            IAnimationChannel channel = this.streamedLipsyncChannels[i];
            channel.UpdateDebugWindow();
        }
        #endif
    }

    void DebugBuffer(PhonemeBuffer.InternalBuffer buffer)
	{
		#if UNITY_EDITOR
		List<PhonemeContainer> phonemes = buffer.GetBufferData();

		Debug.Log ("-----Buffer Start----");
		for (int i = 0; i < phonemes.Count; i++)
		{
			PhonemeContainer phoneme = phonemes[i];

			Debug.Log("Phoneme: " + phoneme.Phoneme);
			Debug.Log("Start: " + phoneme.Start);
			Debug.Log("End: " + phoneme.End);
		}
		#endif
	}

	void RetimeBufferData(List<PhonemeContainer> bufferData, float startTime, List<PhonemeContainer> mergeBuffer)
	{
		for (int i = 0; i < bufferData.Count; i++) 
		{
			PhonemeContainer phoneme = bufferData[i];

			//Retime the phoneme.
			phoneme.Start = phoneme.Start - startTime;
			phoneme.End = phoneme.End - startTime;

			mergeBuffer.Add(phoneme);
		}
	}

	List<PhonemeContainer> CreateMergeBuffer(PhonemeBuffer.InternalBuffer frontBuffer, out float mergeBufferLength)
	{
		List<PhonemeContainer> mergeBuffer = new List<PhonemeContainer>();

		//Start with the buffer furthest back in time (oldest).
		PhonemeBuffer.InternalBuffer current = frontBuffer.GetNextBuffer();

		//Retime all buffers so they use this start value.
		float startTime = current.GetBufferData()[0].Start;

		mergeBufferLength = 0f;

		//step through all buffers until we loop back to the frontbuffer.
		while (!System.Object.ReferenceEquals(current, frontBuffer))
		{
			RetimeBufferData(current.GetBufferData(), startTime, mergeBuffer);

			//Track total length of merge buffer.
			mergeBufferLength += current.GetBufferLength();

			//Traverse buffers upward in time until we loop back to the frontbuffer. (loop breaks there)
			current = current.GetNextBuffer();
		}

		//Finally append lengh of frontbuffer, this gives us the total length of the mergebuffer in seconds.
		mergeBufferLength += frontBuffer.GetBufferLength();

		//Append the frontbuffer at the end of the merged buffer.
		RetimeBufferData(frontBuffer.GetBufferData(), startTime, mergeBuffer);

		return mergeBuffer;
	}
}
