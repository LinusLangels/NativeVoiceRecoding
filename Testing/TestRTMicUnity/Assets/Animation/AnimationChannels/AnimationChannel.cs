using UnityEngine;
using System.Collections;
using System.Collections.Generic;

#if UNITY_EDITOR
using UnityEditor;
#endif

using Newtonsoft.Json.Linq;
using Newtonsoft.Json;

public abstract class AnimationChannel<T> : IAnimationChannel
{
	protected string channelName;
	protected List<T> keyframes;
	protected bool canLoop;
	protected bool canMirror;
	protected bool isStreamed;
	protected bool enabled;
	protected bool mirrorChannel;
    protected bool lipsyncChannel;
    protected int framerate;
    protected Vector3 mirrorAxis;
    protected Vector3 offset;

    #if UNITY_EDITOR
    protected EditorWindow debugWindow;
    protected List<float> debugData;
    #endif

    public bool Enabled { get { return this.enabled; } }

	public AnimationChannel() 
	{
		this.canLoop = true;
		this.canMirror = true;
		this.isStreamed = false;
		this.enabled = true;
	}

	public AnimationChannel(string channelName, List<T> keyframes, bool canLoop, bool canMirror, bool isStreamed, bool lipsyncChannel, int framerate)
	{
		this.channelName = channelName;
		this.keyframes = keyframes;
		this.canLoop = canLoop;
		this.canMirror = canMirror;
		this.isStreamed = isStreamed;
        this.framerate = framerate;
        this.lipsyncChannel = lipsyncChannel;
        this.enabled = true;
        this.offset = Vector3.zero;
	}

	public virtual void Animate(int frameNumber, float transition)
	{
	}

	public virtual void Bind(TransformBase animationTransform)
	{
	}

	public int GetLength()
	{
		return this.keyframes.Count;
	}

	public string GetChannelName()
	{
		return this.channelName;
	}

	public bool CanChannelLoop()
	{
		return this.canLoop;
	}

	public bool CanChannelMirror()
	{
		return this.canMirror;
	}

	public void ToggleChannel(bool toggle)
	{
		this.enabled = toggle;
	}

	public void MirrorChannel(Vector3 axis)
	{
		this.mirrorAxis = axis;
		this.mirrorChannel = true;
	}

    public bool IsLipsyncChannel()
    {
        return this.lipsyncChannel;
    }

    public void AddKeyframe(T keyframe)
	{
		this.keyframes.Add(keyframe);

        #if UNITY_EDITOR
        AddDebugData(keyframe);
        #endif
    }

    public virtual void AddFadeout(float duration)
    {
    }

    public abstract IAnimationChannel Copy();

    public void OffsetChannel(Vector3 offset)
    {
        this.offset = offset;
    }

    protected T CalculateKeyframe(int frameNumber)
	{
		int numberOfKeys = GetLength();
		int useFrame = frameNumber;

		//No need to lookup index in empty keyframe list.
		if (numberOfKeys == 0)
			return default(T);

		if (this.canLoop && frameNumber > (numberOfKeys-1))
			useFrame = frameNumber % (numberOfKeys-1); //wrap current frame to beginning, aka looping.
		else if(!this.canLoop && frameNumber > (numberOfKeys-1))
			useFrame = numberOfKeys-1; //Hold the last frame until instruction has ended.

		return this.keyframes[useFrame];
	}

	#if UNITY_EDITOR
	public void AddDebugWindow(EditorWindow debugWindow)
	{
		this.debugWindow = debugWindow;
		this.debugData = new List<float>();
	}

	public void UpdateDebugWindow()
	{
		if (this.debugWindow != null)
		{
			JObject animationData = new JObject();
			JArray channelDebugData = new JArray();

			foreach (float val in debugData)
			{
				channelDebugData.Add(val);
			}

			animationData[channelName] = channelDebugData;

			Event e = EditorGUIUtility.CommandEvent(animationData.ToString());
			this.debugWindow.SendEvent(e);

			//Clear old data after every update.
			this.debugData.Clear();
		}
	}

	protected abstract void AddDebugData(T data);
	#endif
}

public interface IAnimationChannel
{
	bool Enabled { get; }

	void Animate(int frameNumber, float transition);
	void Bind(TransformBase animationTransform);
	int GetLength();
	string GetChannelName();
	bool CanChannelLoop();
	bool CanChannelMirror();
	void MirrorChannel(Vector3 axis);
    void OffsetChannel(Vector3 offset);
    bool IsLipsyncChannel();
    IAnimationChannel Copy();

    #if UNITY_EDITOR
    void AddDebugWindow(EditorWindow debugWindow);
    void UpdateDebugWindow();
    #endif
}