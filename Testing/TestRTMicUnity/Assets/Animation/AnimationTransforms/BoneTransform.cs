using UnityEngine;
using System.Collections;

public class BoneTransform : TransformBase
{
	protected Transform bone;
	protected Quaternion initialRotation;
    protected Quaternion previousRotation;

    public BoneTransform(string name, Transform bone, Quaternion initialRotation)
	{
		this.name = name;
		this.bone = bone;
		this.initialRotation = initialRotation;
        this.previousRotation = Quaternion.identity;
	}

	public Transform GetBone()
	{
		return this.bone;
	}

	public Quaternion GetInitialRotation()
	{
		return this.initialRotation;
	}

    public Quaternion GetPreviousRotation()
    {
        return this.previousRotation;
    }

    public void SetRotation(Quaternion val, float transition)
    {
        this.bone.localRotation = Quaternion.Lerp(this.previousRotation, val, transition);

        this.previousRotation = bone.localRotation;
    }
}