using UnityEngine;
using System.Collections;

public class TransformBase
{
	protected string name;
	protected bool updated;

	public void ToggleUpdated(bool toggle)
	{
		this.updated = toggle;
	}

	public bool IsTransformUpdated()
	{
		return this.updated;
	}

	public string GetName()
	{
		return this.name;
	}
}