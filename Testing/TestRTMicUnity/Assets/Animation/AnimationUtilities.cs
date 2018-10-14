using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

public enum AnimationType
{
	NoAnimation = 0,
	Dialogue = 1,
	Interaction = 2,
	Locomotion = 3,
	Idle = 4,
	Sound = 5,
	CCAnimation = 6,
	Null = 7,
};

public enum DialogueAnimationType 
{ 
	talk, 
	listen, 
	Null
};

public enum InteractionAnimationType 
{
	Null, 
	Actor, 
	Target,
	Reaction
};
		
[Flags]
public enum Stance : int
{
	Null                = 0,
	StandingNormal      = 0x01,
	StandingTable      	= 0x02,
	SittingNormal       = 0x04,
	SittingAcross    	= 0x08,
	SittingTable     	= 0x10,
	SittingGround       = 0x20,
	LyingNormal         = 0x40,
	SittingBed       	= 0x80,
	StandingLeaning     = 0x100,
	UseStanding = StandingNormal | SittingAcross | StandingTable | StandingLeaning,
	UseSitting = SittingNormal | SittingGround | SittingBed | SittingTable | LyingNormal,
};

public enum StandardAnimations
{
	Idle,
	NoAnimation,
	EmptyState,
	IntroCameraAnimation,
}

public enum FrameRate
{
	FPS30 = 30,
	FPS60 = 60,
}

[Flags]
public enum AnimationFlags
{
	None = 0,
	FilterXCurve = 1,
	FilterYCurve = 2,
	FilterZCurve = 4,
}

public class AnimationUtilities 
{
	static Dictionary<Stance, string> MappedStances;
	static Dictionary<string, Stance> ReversedMappedStances;
	static Dictionary<StandardAnimations, string> MappedAnimations;
	static Dictionary<string, StandardAnimations> ReversedMappedAnimations;

	//Do setup in static constructor.
	static AnimationUtilities()
	{
		//Precache all enums to strings to avoid heavy typecasting later on. This gives us type safety and fast string access.
		MappedStances = new Dictionary<Stance, string>();
		MappedAnimations = new Dictionary<StandardAnimations, string>();

		foreach (Stance stance in Enum.GetValues(typeof(Stance)))
		{
			MappedStances.Add(stance, stance.ToString());
		}

		foreach (StandardAnimations animation in Enum.GetValues(typeof(StandardAnimations)))
		{
			MappedAnimations.Add(animation, animation.ToString());
		}

		//Build reverse TKey, TValue mapping.
		ReversedMappedStances = MappedStances.ToDictionary(x => x.Value, x => x.Key);
		ReversedMappedAnimations = MappedAnimations.ToDictionary(x => x.Value, x => x.Key);
	}

	public static string BuildStateInfo(AnimationType type, string actionID, string stance, InteractionAnimationType interactionType = InteractionAnimationType.Null, bool mirror = false) //skeleton for later missdeads
	{
		string state = type.ToString() + stance + "." + actionID;

		if (interactionType != InteractionAnimationType.Null)
			state += "_" + interactionType.ToString().ToLower();
		if (mirror)
			state += "_mirror";
		
		return state;
	}

	public static float CalculatePreviewTime(MetaInfoItem info)
	{
		float timeInPercent = 0f;

		if (info != null)
		{
			int frameRange = info.EndFrame - info.StartFrame;
			int previewFrame = info.PreviewFrame - info.StartFrame;
			timeInPercent = (float)previewFrame / (float)frameRange;
		}
		
		return timeInPercent;
	}
	
	public static float CalculateTransitionTime(MetaInfoItem info, float animationLength)
	{
		float transitionTime = (float)info.BlendInFrames / 30f;
		
		return transitionTime;
	}
	
	public static float CalculateExitTime(MetaInfoItem info)
	{
		int frameRange = info.EndFrame - info.StartFrame;
		int exitFrame = info.ExitFrame - info.StartFrame;
		float exitTime = (float)exitFrame / (float)frameRange;
		
		return exitTime;
	}
	
	public static float CalculateAnimationLength(MetaInfoItem info, int exitFrame)
	{
		float timeInSeconds = 0f;

		if (info != null)
		{
			int frameRange = info.EndFrame - info.StartFrame;
			int exitRange = frameRange - exitFrame;
			float exitTime = (float)exitRange / (float)frameRange;
			timeInSeconds = (frameRange / 30f) * exitTime;
		}
		
		return timeInSeconds;
	}

	public static float CalculateSilenceAnimationLength(MetaInfoItem info)
	{
		float timeInSeconds = info.SilenceFrames / 30f;
		
		return timeInSeconds;
	}

	public static IEnumerator PlayAndWaitForFinish(Animation animation, string animationName, float weight, float speed, float threshold)
	{
		AnimationState state = animation[animationName];
		state.wrapMode = WrapMode.ClampForever;
		state.weight = weight;
		state.speed = speed;
		var t = state.time;

		while((t/state.length) + float.Epsilon < threshold)
		{
			if (animation == null)
				break;

			state.enabled = true;
			animation[animationName].time = t;
			animation.Sample();
			state.enabled = false;

			t += Time.deltaTime * state.speed;
			yield return null;
		}

		state.weight = 0f;
		state.enabled = false;
	}

	public static MetaInfoItem GetListenMeta(MetaInfoItem dialogueMeta)
	{
		try
		{
			MetaInfoItem meta = null;

			foreach(MetaInfoItem listenMeta in AnimationMetaDatabase.Instance.listenDatabase)
			{
				if (dialogueMeta.Attitude == listenMeta.Attitude)
				{
					meta = listenMeta;
					break;
				}
			}

			if (meta != null)
				return meta;
			else
			{
				MetaInfoItem listenMeta = AnimationMetaDatabase.Instance.GetMeta("listen", "nonespecific", "StandingNormal");

				if (listenMeta != null)
					return listenMeta;
				else
				{
					//CrashHelp.Report("AnimationUtilities, GetListenMeta", "Listen meta for: " + dialogueMeta.ID + " not found.");
				}
			}
		}
		catch (Exception e)
		{
			//CrashHelp.Report("DialogAnimationMetaDatabase", e.Message, e);
		}
		return null;
		
	}

	public static float ConvertFramesToSeconds(int frameCount, FrameRate frameRate)
	{
		return ((float)frameCount / (float)frameRate);
	}

	public static string StanceEnumToString(Stance stance)
	{
		if (MappedStances.ContainsKey(stance))
			return MappedStances[stance];

		return string.Empty;
	}

	public static Stance StanceStringToEnum(string stance)
	{
		if (string.IsNullOrEmpty(stance))
			return Stance.Null;

		if (ReversedMappedStances.ContainsKey(stance))
			return ReversedMappedStances[stance];
		
		return Stance.Null;
	}

	public static string AnimationEnumToString(StandardAnimations animation)
	{
		if (MappedAnimations.ContainsKey(animation))
			return MappedAnimations[animation];
		
		return string.Empty;
	}

	public static StandardAnimations AnimationStringToEnum(string animation)
	{
		if (ReversedMappedAnimations.ContainsKey(animation))
			return ReversedMappedAnimations[animation];
		
		return StandardAnimations.NoAnimation;
	}
}
