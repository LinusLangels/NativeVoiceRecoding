using UnityEngine;
using System.Collections;

public class AdditiveAnimation : MonoBehaviour 
{
	public string AdditiveAnimationName;

	public void Initialize(string name) 
	{
		this.AdditiveAnimationName = name;
	}

	public void EnableAdditiveAnimation(Animator animator)
	{
		animator.SetLayerWeight(5, 1f);
		animator.SetLayerWeight(6, 1f);

		if (!animator.GetCurrentAnimatorStateInfo(5).IsName(this.AdditiveAnimationName))
			animator.Play(this.AdditiveAnimationName, 5, 0.1f);

		if (!animator.GetCurrentAnimatorStateInfo(6).IsName(this.AdditiveAnimationName))
			animator.Play(this.AdditiveAnimationName, 6, 0.1f);
	}
}
