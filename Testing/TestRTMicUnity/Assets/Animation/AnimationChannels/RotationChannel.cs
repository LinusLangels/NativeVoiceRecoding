using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public class RotationChannel : AnimationChannel<Vector3> 
{
	private BoneTransform boneTransform;

	public RotationChannel(string channelName, List<Vector3> keyframes, bool canLoop, bool canMirror, bool isStreamed, bool lipsyncChannel, int framerate) : 
		base(channelName, keyframes, canLoop, canMirror, isStreamed, lipsyncChannel, framerate)
	{
	}

	public override void Animate(int frameNumber, float transition)
	{
        if (this.boneTransform == null)
            return;

        this.boneTransform.ToggleUpdated(true);

		Vector3 keyValue = CalculateKeyframe(frameNumber);

        //Factor in any offset setting the caller may have specified.
        keyValue = new Vector3(keyValue.x + offset.x, 
                               keyValue.y + offset.y, 
                               keyValue.z + offset.z);

		if (this.canMirror && this.mirrorChannel)
		{
			 Vector3 newKey = new Vector3(keyValue.x * this.mirrorAxis.x, 
								          keyValue.y * this.mirrorAxis.y,
								          keyValue.z * this.mirrorAxis.z);

            keyValue = newKey;
		}

		Quaternion newRot = Quaternion.Euler(this.boneTransform.GetInitialRotation().eulerAngles - keyValue);

        this.boneTransform.SetRotation(newRot, transition);
	}

	public override void Bind(TransformBase animationTransform)
	{
		this.boneTransform = animationTransform as BoneTransform;
	}

	public Quaternion GetPreviousRotation()
	{
        return this.boneTransform.GetPreviousRotation();
	}

    public override void AddFadeout(float duration)
    {
        int numberOfFrames = (int)(this.framerate * duration);

        Vector3 lastValue = Vector3.zero;
        if (this.keyframes != null && this.keyframes.Count > 0)
        {
            lastValue = this.keyframes[this.keyframes.Count - 1];
        }

        float scale = 0f;
        float frame = (float)numberOfFrames;

        for (int i = 0; i < numberOfFrames; i++)
        {
            float scaling = Mathf.Clamp(1.0f - (scale / frame), 0.5f, 1.0f);
            Vector3 fadedKey = new Vector3(lastValue.x *scaling, lastValue.y * scaling, lastValue.z * scaling);
            this.keyframes.Add(fadedKey);  

            scale += 1f;
        }
    }

    public override IAnimationChannel Copy()
    {
        List<Vector3> copiedKeyframes = new List<Vector3>(this.keyframes.ToArray());
        IAnimationChannel copiedRotationChannel = new RotationChannel(this.channelName, copiedKeyframes, this.canLoop, this.canMirror, this.isStreamed, this.lipsyncChannel, this.framerate);

        return copiedRotationChannel;
    }

	#if UNITY_EDITOR
    protected override void AddDebugData(Vector3 data)
    {
        if (this.debugData != null)
            this.debugData.Add(data.z);
    }
	#endif
}