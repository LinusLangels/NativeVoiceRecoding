using UnityEngine;
using System.Collections;

public class MorphTransform : TransformBase
{
	protected SkinnedMeshRenderer mesh;
	protected int index;

	public MorphTransform(string name, SkinnedMeshRenderer mesh, int index)
	{
		this.name = name;
		this.mesh = mesh;
		this.index = index;
	}

	public SkinnedMeshRenderer GetMesh()
	{
		return this.mesh;
	}

	public int GetIndex()
	{
		return this.index;
	}
}