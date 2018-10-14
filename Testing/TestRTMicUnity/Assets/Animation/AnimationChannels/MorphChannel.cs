using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class MorphChannel : AnimationChannel<float> 
{
	private MorphTransform morphTransform;

	public MorphChannel(string channelName, List<float> keyframes, bool canLoop, bool canMirror, bool isStreamed, bool lipsyncChannel, int framerate) :
		base(channelName, keyframes, canLoop, canMirror, isStreamed, lipsyncChannel, framerate)
	{
	}

	public override void Animate(int frameNumber, float transition)
	{
        if (this.morphTransform == null)
            return;

        SkinnedMeshRenderer mesh = morphTransform.GetMesh();

        if (mesh != null)
        {
            //Allows us to check if this channel was updates this frame.
            morphTransform.ToggleUpdated(true);

            float keyValue = CalculateKeyframe(frameNumber);

            int blendshapeIndex = morphTransform.GetIndex();
            float finalBlendValue = Mathf.SmoothStep(mesh.GetBlendShapeWeight(blendshapeIndex), keyValue, transition);
            mesh.SetBlendShapeWeight(blendshapeIndex, finalBlendValue);
        }
	}

	public override void Bind(TransformBase animationTransform)
	{
		this.morphTransform = animationTransform as MorphTransform;
	}

    public override void AddFadeout(float duration)
    {
        int numberOfFrames = (int)(this.framerate * duration);

        float lastValue = 0f;
        if (this.keyframes != null && this.keyframes.Count > 0)
        {
           lastValue =  this.keyframes[this.keyframes.Count - 1];
        }

        float scale = 0f;
        float frame = (float)numberOfFrames;

        for (int i = 0; i < numberOfFrames; i++)
        {
            float scaling = Mathf.Clamp(1.0f - (scale / frame), 0.5f, 1.0f);
            float fadedKey = lastValue * scaling;
            this.keyframes.Add(fadedKey);

            scale += 1f;
        }
    }

    public override IAnimationChannel Copy()
    {
        List<float> copiedKeyframes = new List<float>(this.keyframes.ToArray());
        IAnimationChannel copiedMorphChannel = new MorphChannel(this.channelName, copiedKeyframes, this.canLoop, this.canMirror, this.isStreamed, this.lipsyncChannel, this.framerate);

        return copiedMorphChannel;
    }

	#if UNITY_EDITOR
    protected override void AddDebugData(float data)
    {
        if (this.debugData != null)
            this.debugData.Add(data);
    }
	#endif
}