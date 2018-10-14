using UnityEngine;
using System;
using System.Reflection;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;

public class FacialAnimation : MonoBehaviour 
{
	public class ChannelState
	{
		private string channelName;
		private bool channelAdded;

		public ChannelState(string name)
		{
			this.channelName = name;
			this.channelAdded = false;
		}

		public string GetChannelName()
		{
			return this.channelName;
		}

		public void ToggleChannelAdded(bool toggle)
		{
			this.channelAdded = toggle;
		}
	}

    public static readonly int FRAMERATE = 30;

    private FacialAnimationClip previousClip = null;
	private FacialAnimationClip currentClip = null;

    //Used to debug in the editor.
    public string CurrentClipName;

	//The transforms we use to retarget/bind an animation clip to the actual data being driven.
	private Dictionary<string, TransformBase> animationTransforms = new Dictionary<string, TransformBase>();

	//Optimization, lookup bone names, so we can iterate faster.
	private List<TransformBase> transformLookup;

	//When a clip does not use all channels we inject these at the empty slots to fade those transforms out.
	private List<IAnimationChannel> baseChannels;

    private SkinnedMeshRenderer targetMesh;
		
	void Awake()
	{
	}

    public void SetTargetMesh(SkinnedMeshRenderer target)
    {
        this.targetMesh = target;

        FindBoneTransforms();
        FindMorphTransforms();

        //Optimized lookup table to iterate all transform.
        BuildLookupForTransforms();

        //Used to populate animationclips when first created.
        CreateBaseChannels();
    }

	void CreateBaseChannels()
	{
		this.baseChannels = new List<IAnimationChannel>();

		for (int i = 0; i < this.transformLookup.Count; i++)
		{
			TransformBase transform = this.transformLookup[i];

			if (transform is MorphTransform)
			{
				//A small list of keyframes, used to sample values.
				List<float> keys = new List<float>(5) { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
				MorphChannel morphChannel = new MorphChannel(transform.GetName(), keys, false, false, false, false, FRAMERATE);

				this.baseChannels.Add(morphChannel);
			}
			else if (transform is BoneTransform)
			{
				BoneTransform boneTransform = transform as BoneTransform;

				//Sample initial rotation value.
				Vector3 angle = boneTransform.GetInitialRotation().eulerAngles;
				List<Vector3> keys = new List<Vector3>(5) { angle, angle, angle, angle, angle };
				RotationChannel rotationChannel = new RotationChannel(boneTransform.GetName(), keys, false, false, false, false, FRAMERATE);

				this.baseChannels.Add(rotationChannel);
			}
		}
	}
	
	void FindBoneTransforms()
	{
		string[] optionalBones = {
			"Mid_Head_Jnt_03", 
			"L_LowEyelid_Jnt_00", 
			"R_LowEyelid_Jnt_00", 
			"L_UpEyelid_Jnt_00", 
			"R_UpEyelid_Jnt_00", 
			"L_Eyeball_Jnt_00", 
			"R_Eyeball_Jnt_00"
		};

		Dictionary<string, Transform> mappedTransforms = new Dictionary<string, Transform>();
		TransformHelp.FindChildrenRecursive(this.transform, optionalBones, mappedTransforms);

		//Avoid using enumerators when iterating
		foreach (KeyValuePair<string, Transform> pair in mappedTransforms)
		{
			BoneTransform boneTransform = new BoneTransform(pair.Key, pair.Value, pair.Value.localRotation);

            if (!this.animationTransforms.ContainsKey(pair.Key))
                this.animationTransforms.Add(pair.Key, boneTransform);
		}
	}

	void FindMorphTransforms()
	{
		if (this.targetMesh != null)
		{
			if (targetMesh.sharedMesh.blendShapeCount == 0)
				UnityEngine.Debug.LogError("Couldn't find characters blend shapes.");

			else
			{
				for (int i = 0; i < targetMesh.sharedMesh.blendShapeCount; i++)
				{
					string name = targetMesh.sharedMesh.GetBlendShapeName(i);
					string[] namePartition = name.Split('.');
					name = namePartition[namePartition.Length-1];

					//Do not track this blendshape.
					if (name != "BaseHeadGeo" && name != "BaseHead")
					{
						MorphTransform morphTransform = new MorphTransform(name, targetMesh, i);

						if (this.animationTransforms.ContainsKey(name))
							this.animationTransforms[name] = morphTransform;
						else
							this.animationTransforms.Add(name, morphTransform);
					}
					else
					{
						targetMesh.SetBlendShapeWeight(i, 100f); //Activate character head
					}
				}
			}
		}
	}

    void BuildLookupForTransforms()
    {
        this.transformLookup = new List<TransformBase>();

        foreach (KeyValuePair<string, TransformBase> pair in this.animationTransforms)
        {
            this.transformLookup.Add(pair.Value);
        }
    }

    void LateUpdate()
	{
		if (this.currentClip != null)
		{
			if (this.currentClip.IsPlaying())
			{
				this.currentClip.UpdateFrame();

				//Once animation update is done we reset state on all transforms.
				ResetTransforms();
			}
		}
	}

	void ResetTransforms()
	{
		//Sanity check
		if (this.transformLookup == null)
			return;

		//Reset update state of all transform after each update.
		for (int i = 0; i < this.transformLookup.Count; i ++)
		{
			this.transformLookup[i].ToggleUpdated(false);
		}
	}

	//Fill clip with animation channels from animation data from the database.
	FacialAnimationClip FillAnimationClip(FacialAnimationData data)
	{
		//Create animation clip to fill.
		FacialAnimationClip clip = new FacialAnimationClip(data.ID, this.baseChannels, FRAMERATE); 

		for (int i = 0; i < data.RotationChannels.Count; i++)
		{
			RotationFacialAnimationChannel channelData = data.RotationChannels[i];

			RotationChannel rotationChannel = new RotationChannel(channelData.ChannelName, channelData.Keys, channelData.CanLoop, channelData.CanMirror, false, false, FRAMERATE);
			clip.AddAnimationChannel(rotationChannel);
		}

		for (int i = 0; i < data.MorphChannels.Count; i++)
		{
			MorphFacialAnimationChannel channelData = data.MorphChannels[i];
			MorphChannel morphChannel = new MorphChannel(channelData.ChannelName, channelData.Keys, channelData.CanLoop, channelData.CanMirror, false, false, FRAMERATE);
            clip.AddAnimationChannel(morphChannel);
        }

		return clip;
	}

	public Quaternion GetLastRotationForChannel(string channelName)
	{
		Quaternion previousRotation = Quaternion.identity;

		if (this.previousClip != null)
		{
			IAnimationChannel previousChannel = this.previousClip.GetAnimationChannel(channelName);

			if (previousChannel != null && previousChannel is RotationChannel)
			{
				RotationChannel previousRotationChannel = previousChannel as RotationChannel;
				previousRotation = previousRotationChannel.GetPreviousRotation();
			}
		}
		else
		{
			//If we dont have a previous clip to extract a rotation from we use the initial transform for this channel.
			if (this.animationTransforms.ContainsKey(channelName))
			{
				BoneTransform boneTransform = this.animationTransforms[channelName] as BoneTransform;
				previousRotation = boneTransform.GetInitialRotation();
			}
		}

		return previousRotation;
	}

	public FacialAnimationClip ConstructFacialAnimationClip(string type, string actionID, string stance, InteractionAnimationType interactionType = InteractionAnimationType.Null)
	{
		FacialAnimationData data = FacialAnimationDatabase.Instance.GetFacialAnimation(type, actionID, interactionType, AnimationUtilities.StanceStringToEnum(stance));

		if (data != null)
		{
			FacialAnimationClip clip = FillAnimationClip(data);

			return clip;
		}

		return null;
	}
	
	public void PlayFacialAnimation(FacialAnimationClip animationClip, bool doTransition, float transitionTime, float playTime)
	{
		if (animationClip != null)
		{
			this.currentClip = animationClip;
            this.CurrentClipName = this.currentClip.AnimationName;

			//Make sure the channels we added are bound to their transforms. (the actual thing they are to animate)
			this.currentClip.BindAllChannels(this.animationTransforms);

			//Set clip to currently active clip, it now awaits update calls.
			this.currentClip.PlayAnimation(doTransition ? 0f : 1f, transitionTime, playTime);
            this.currentClip.UpdateFrame();

            this.previousClip = this.currentClip;
		}
		else
		{
			Debug.LogError("Cannot play animation clip");
		}
	}

	public void StopFacialAnimation()
	{
		if (this.currentClip != null)
			this.currentClip.StopAnimation();
	}
	
	public void PreviewFacialAnimation(FacialAnimationClip animationClip, float previewTime)
	{
		if (animationClip != null)
		{
            this.CurrentClipName = animationClip.AnimationName;

            //Make sure the channels we added are bound to their transforms. (the actual thing they are to animate)
            animationClip.BindAllChannels(this.animationTransforms);

            //Set clip state to playing, it now awaits update calls.
            animationClip.PlayAnimation(1f, 0f, previewTime);

			//Update all transforms to show the frame specified in previewTime.
			animationClip.UpdateFrame();
		}
		else
		{
			Debug.LogError("Cannot preview animation clip");
		}
	}
}