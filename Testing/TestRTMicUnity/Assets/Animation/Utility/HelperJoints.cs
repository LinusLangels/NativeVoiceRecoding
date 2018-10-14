using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public class HelperJoints : MonoBehaviour 
{	
	[Serializable]
	public class AttachJointObjects
	{
		[SerializeField]
		public GameObject jointGO;
		[SerializeField]
		public int attachedID;
		
		public AttachJointObjects(GameObject jointGO, int id)
		{
			this.jointGO = jointGO;
			this.attachedID = id;
		}                  
	}
	public bool EnableHelpers = false;
	
	public List<HelperJoints.AttachJointObjects> attachJoints  = new List<HelperJoints.AttachJointObjects>();
	SkinnedMeshRenderer meshRenderer = null;
	//Allocation of variables to be run in loop, created here to save on heap space.
	int[] vertIDs = null;
	Transform[] joints = null;
	Vector3 pos = Vector3.zero;
	Vector3 sideRot = Vector3.zero;
	Vector3 forwardRot = Vector3.zero;
	Vector3 upRot = Vector3.zero;
	Matrix4x4 matrix = new Matrix4x4();
	Mesh baked = new Mesh();
	
	public void AppendJoints(List<HelperJoints.AttachJointObjects> jointObjs)
	{
		this.attachJoints.AddRange(jointObjs);
	}
	
	public void CollectOptimizedData()
	{
		int[] IDs = new int[attachJoints.Count];
		Transform[] jointTransforms = new Transform[attachJoints.Count];
		
		for (int i = 0; i < attachJoints.Count; i++)
		{
			if (attachJoints[i].jointGO != null)
			{
				IDs[i] = attachJoints[i].attachedID;
				jointTransforms[i] = attachJoints[i].jointGO.transform;
			}
		}
		
		vertIDs = IDs;
		joints = jointTransforms;
	}
	
	void Start () 
	{
		meshRenderer = this.GetComponent<SkinnedMeshRenderer>();
		
		if (meshRenderer != null && this.attachJoints != null && this.attachJoints.Count > 0)
		{
			this.baked = new Mesh();
			CollectOptimizedData();
			this.EnableHelpers = true;
		}
	}

	private static Quaternion GetRotation(Matrix4x4 matrix)
	{
		var qw = Mathf.Sqrt(1f + matrix.m00 + matrix.m11 + matrix.m22) / 2;
		var w = 4 * qw;
		var qx = (matrix.m21 - matrix.m12) / w;
		var qy = (matrix.m02 - matrix.m20) / w;
		var qz = (matrix.m10 - matrix.m01) / w;

		return new Quaternion(qx, qy, qz, qw);
	}

	private static bool IsInfinity(Quaternion q) 
	{
		return float.IsInfinity(q.x) || float.IsInfinity(q.y) || float.IsInfinity(q.z);
	}
	
	void LateUpdate() 
	{
		if (this.EnableHelpers)
		{
			meshRenderer.BakeMesh(baked); //fill the mesh object with vert/normal/triangle information from the skinned mesh
			Vector3[] cachedVerts = baked.vertices;
			Vector3[] cachedNormals = baked.normals;
			Vector4[] cachedTangents = baked.tangents;

			for (int i = 0; i < vertIDs.Length; i++) //loop over our arrays and assign the correct positon and rotation to the joints
			{
				int ID = vertIDs[i];

				pos = transform.TransformPoint( cachedVerts[ID] );
				sideRot = -transform.TransformDirection( cachedTangents[ID] ); //Just flip it so we align to tangent
				forwardRot = transform.TransformDirection( cachedNormals[ID] );
				Vector3.OrthoNormalize(ref sideRot, ref forwardRot);
				upRot = Vector3.Cross(forwardRot, sideRot);

//				Debug.DrawRay(pos, sideRot * 0.1f, Color.red);
//				Debug.DrawRay(pos, upRot * 0.1f, Color.yellow);
//				Debug.DrawRay(pos, forwardRot * 0.1f, Color.blue);

				//Set Position of joint
				joints[i].position = pos;

				//Set Rotation of joint
				matrix.SetColumn(0, sideRot); 
				matrix.SetColumn(1, upRot);
				matrix.SetColumn(2, forwardRot);
				matrix.SetColumn(3, new Vector4(0, 0, 0, 1));
				Quaternion rotation = GetRotation(matrix);
				if (!IsInfinity(rotation))
					joints[i].rotation = GetRotation(matrix);
			}
		}
	}
}