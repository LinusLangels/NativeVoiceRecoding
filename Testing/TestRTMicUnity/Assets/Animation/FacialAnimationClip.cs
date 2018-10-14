using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

public class FacialAnimationClip 
{
	public string AnimationName { get; private set; }

	//Internal state variables.
	protected List<IAnimationChannel> animationChannels;
	protected float playedTime;
	protected float maxChannelLength;
	protected bool playing;
	protected int currentFrame;
	protected float transition;
	protected float transitionTime;
    protected int framerate;

	public FacialAnimationClip(string animationName, List<IAnimationChannel> baseChannels, int framerate)
	{
		this.AnimationName = animationName;
		this.animationChannels = new List<IAnimationChannel>(baseChannels);
        this.framerate = framerate;
	}

	public void AddAnimationChannel(IAnimationChannel channel)
	{
		if (this.animationChannels != null)
		{
			//Remove previous entry if there is one.
			RemoveAnimationChannel(channel.GetChannelName());

			//Add channel to our iterator list.
			this.animationChannels.Add(channel);

			//Find the channel that is the longest within this animation clip, that channel determines the clips total length.
			float channelLength = (float)((float)channel.GetLength() / this.framerate);
			if (channelLength > this.maxChannelLength)
			{
				this.maxChannelLength = channelLength;
			}
		}
	}

	public void RemoveAnimationChannel(string channelName)
	{
		IAnimationChannel channel = null;

		for (int i = 0; i < this.animationChannels.Count; i ++)
		{
			if (string.CompareOrdinal(this.animationChannels[i].GetChannelName(), channelName) == 0)
			{
				channel = this.animationChannels[i];
				break;
			}
		}

		if (channel != null)
			this.animationChannels.Remove(channel);
	}

	public IAnimationChannel GetAnimationChannel(string channelName)
	{
		for (int i = 0; i < this.animationChannels.Count; i ++)
		{
			IAnimationChannel channel = this.animationChannels[i];

			if (string.CompareOrdinal(channel.GetChannelName(), channelName) == 0)
			{
				return channel;
			}
		}

		return null;
	}

    public List<IAnimationChannel> GetAllAnimationChannels()
    {
        return this.animationChannels;
    }

	public void BindAllChannels(Dictionary<string, TransformBase> animationTransforms)
	{
		for (int i = 0; i < this.animationChannels.Count; i ++)
		{
			IAnimationChannel channel = this.animationChannels[i];
			string channelName = channel.GetChannelName();

			//Bind channel to a transform.
			if (animationTransforms.ContainsKey(channelName))
			{
				TransformBase animationTransform = animationTransforms[channelName];

				//Bind channel to animatable transform.
				channel.Bind(animationTransform);
			}
		}
	}

	public void MirrorAllChannels(Vector3 axis)
	{
		for (int i = 0; i < this.animationChannels.Count; i ++)
		{
			this.animationChannels[i].MirrorChannel(axis);
		}
	}

    public int GetCurrentFrame()
    {
        return this.currentFrame;
    }

	public bool IsPlaying()
	{
		return this.playing;
	}

	public float PlayedTime()
	{
		return this.playedTime;
	}

	public float Length()
	{
		return this.maxChannelLength;
	}

	public void UpdateFrame()
	{
		if (this.playing)
		{
			this.currentFrame = (int)(this.playedTime * this.framerate);

			//Animate all channels for this frametick.
			for (int i = 0; i < this.animationChannels.Count; i ++)
			{
				if (this.animationChannels[i].Enabled)
				{
					this.animationChannels[i].Animate(this.currentFrame, this.transition);
				}
			}
		
			this.playedTime += Time.deltaTime;
			this.transition += Time.deltaTime / this.transitionTime;
			this.transition = Mathf.Clamp01(this.transition);
		}
	}
		
	public void PlayAnimation(float transition, float transitionTime, float playTime)
	{
		this.transition = transition;
		this.transitionTime = transitionTime;
		this.playedTime = playTime;
		this.playing = true;
		this.currentFrame = (int)(this.framerate * playTime);
	}

	public void StopAnimation()
	{
		this.playing = false;
	}
}
