using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;
using Plotagon.Serialization;

/**
 * Lip Sync Data, is an object that calculates customised data for our lip-synchronisation
 * 
 * This class is a conversion from Jonas Beskows application written in c++.
 */
public class LipSyncData
{
	/*
	Point - 2d time-value pair, used to describe targets etc.
	*/
	public struct Point
	{
		public long t;
		public float v;
		
		public Point(long _t, float _v)
		{
			t = _t;
			v = _v;
		}
	}
	
	/*
		PhoneDefine - segment definition, contains prototypical target values for a phone, for one parameter
	*/
	public class PhoneDefine
	{
		public Dictionary<float, float> targets = new Dictionary<float, float>();
		public bool isGesture;
	}
	
	/*
		Parameter - holds N phonedefs and a time constant for a parameter
	*/
	public class Parameter
	{
		public Dictionary<string, PhoneDefine> defs = new Dictionary<string, PhoneDefine>();
		public float maxvel = 0;
		public float maxacc = 0;	
	}
		
	public static readonly float LipFadeOutTime = 0.0f;

	List<Track> tracks;
	Dictionary<string, string> symMap;
	Dictionary<int, List<float>> parMap;

	//Lipsync animation channels.
	private RotationChannel jawChannel;
	private MorphChannel cornerinChannel;
	private MorphChannel iChannel;
	private MorphChannel lowerUpChannel;
	private MorphChannel lowerDownChannel;
	private MorphChannel upperUpChannel;
	private MorphChannel upperDownChannel;

    public LipSyncData()
	{
		this.symMap = SymMap.FillMap();
	}

	public void SetLipSyncChannels(List<IAnimationChannel> animationChannels)
	{
        for (int i = 0; i < animationChannels.Count; i++)
        {
            IAnimationChannel channel = animationChannels[i];
            string channelName = channel.GetChannelName();

            switch (channelName)
            {
                case "Mid_Head_Jnt_03":
                    jawChannel = channel as RotationChannel;
                    break;
                case "Corner_In":
                    cornerinChannel = channel as MorphChannel;
                    break;
                case "I":
                    iChannel = channel as MorphChannel;
                    break;
                case "Lip_LowerUp":
                    lowerUpChannel = channel as MorphChannel;
                    break;
                case "Lip_LowerDown":
                    lowerDownChannel = channel as MorphChannel;
                    break;
                case "Lip_UpperUp":
                    upperUpChannel = channel as MorphChannel;
                    break;
                case "Lip_UpperDown":
                    upperDownChannel = channel as MorphChannel;
                    break;
            }
        }
    }

	public void StreamedTextToSpeech(List<PhonemeContainer> phonemes, float streamOffset)
	{
        if (phonemes.Count > 0)
		{
			this.tracks = PhoneDefines.FillTracks();
			this.parMap = ParMap.FillMap();

			for (int i = 0; i < phonemes.Count; i++)
			{
				PhonemeContainer phoneme = phonemes[i];

				//Push phoneme data into tracks.
				AddSym(phoneme.Phoneme, (int)(phoneme.Start * 1000.0f));
			}

			for (int i = 0; i < this.tracks.Count; i++)
			{
				Track track = this.tracks[i];
				track.PushPointsToVector();
			}

			//Define time when to stop calculating.
			float time = phonemes[phonemes.Count - 1].End + LipFadeOutTime;

			//Make sure frame spacing is in 30fps.
			float step = (1000.0f / 30.0f);

			//Scale to milliseconds.
			streamOffset = streamOffset * 1000.0f;

			for (float t = 0; t < (float)(time * 1000); t += step)
			{
				//Matrix row for this frame.
				List<float> animationFrame = GetFrame(t);

				//If time is less than streamoffset dont append new animation data into channels.
				if (t < streamOffset)
					continue;

				//Reference go animation frame output.
				//0 = Mid_Head_Jnt_03
				//1 = Corner_In >= 0      1 = I < 0
				//2 = lowerLipLifter >= 0   lowerLipLowerer < 0
				//3 = upperLipLowerer >= 0 upperLipLifter < 0

				float jaw = animationFrame[0]+1.0f;

				jaw = Mathf.Lerp(3.0f, -5f, Mathf.Abs(jaw)); 
				
				float WQ = animationFrame[1]*100f;
				float lowerLip = animationFrame[2]*100f;
				float upperLip = animationFrame[3]*100f;

				this.jawChannel.AddKeyframe(new Vector3(0f, 0f, jaw));

				if (WQ >= 0f)
				{
                    this.cornerinChannel.AddKeyframe(WQ);
				}
				else
				{
                    this.cornerinChannel.AddKeyframe(0f);
				}
				
				if (WQ < 0f)
				{
                    this.iChannel.AddKeyframe(WQ * 0.75f);
				}
				else
				{
                    this.iChannel.AddKeyframe(0f);
				}

				if (lowerLip >= 0f)
				{
                    this.lowerUpChannel.AddKeyframe(lowerLip*1.1f);
				}
				else
				{
                    this.lowerUpChannel.AddKeyframe(0f);
				}

				if (lowerLip < 0f)
				{
                    this.lowerDownChannel.AddKeyframe(lowerLip);
				}
				else
				{
                    this.lowerDownChannel.AddKeyframe(0f);
				}

				if (upperLip >= 0f)
				{
                    this.upperDownChannel.AddKeyframe(upperLip);
				}
				else
				{
                    this.upperDownChannel.AddKeyframe(0f);
				}

				if (upperLip < 0f)
				{
                    this.upperUpChannel.AddKeyframe(upperLip);
				}
				else
				{
                    this.upperUpChannel.AddKeyframe(0f);
				}
			}
		}
	}

	public void AddSym(string sym, float time)
	{
		//Remap phoneme into our internal keyword.
		if (this.symMap.ContainsKey(sym))
			sym = this.symMap[sym];
		
		foreach(Track t in this.tracks)
			t.AddPhone(sym, (long)time);	
	}
	
	public List<float> MapFrame(List<float> x) 
	{
		List<float> y = new List<float>();
		if (this.parMap.Count <= 0)
			return y;
		
		if (x.Count != this.parMap.Count - 1)
		{
			UnityEngine.Debug.LogWarning("Frame size and parmap mismatch");
			return y;
		}
		
		for (int i=0; i < this.parMap[0].Count; i++)
		{
			y.Add(this.parMap[0][i]);

			for (int j = 1; j < this.parMap.Count; j++)
			{
				y[i] += x[j - 1] * this.parMap[j][i];
			}
		}
		
		return y;
	}
	
	public List<float> GetFrame(float time)
	{
		List<float> res = new List<float>();
		
		foreach(Track t in this.tracks)
			res.Add(t.GetFilteredValue((long)time));

		//Remap out calculated value onto a column of our choosing.
		if (this.parMap.Count != 0)
			return MapFrame(res);
		else 
			return res;
	}
}