using UnityEngine;

using System;
using System.Collections;

public static class MathHelp 
{
	public static Vector3 RoundOfVector3(Vector3 inputVector, int roundTo)
	{
		return new Vector3((float)System.Math.Round(inputVector.x, roundTo), (float)System.Math.Round(inputVector.y, roundTo), (float)System.Math.Round(inputVector.z, roundTo));
	}

	public static bool FloatNearlyEqual(float a, float b, double epsilon)
	{
		float absA = Math.Abs(a);
		float absB = Math.Abs(b);
		float diff = Math.Abs(a - b);

		// shortcut, handles infinities
		if (a == b)
		{ 
			return true;
		} 
		else if (a == 0 || b == 0 || diff < float.MinValue) 
		{
			// a or b is zero or both are extremely close to it
			// relative error is less meaningful here
			return diff < (epsilon * float.MinValue);
		}
		else
		{ 
			// use relative error
			return diff / (absA + absB) < epsilon;
		}
	}

	public static bool Vector3NearlyEqual(Vector3 originalVector, Vector3 newVector, float epsilon)
	{
		bool x = FloatNearlyEqual(originalVector.x, newVector.x, epsilon);
		bool y = FloatNearlyEqual(originalVector.y, newVector.y, epsilon);
		bool z = FloatNearlyEqual(originalVector.z, newVector.z, epsilon);

		bool equal = x;
		equal &= y;
		equal &= z;

		return equal;

	}

	public static Vector3 HalfAngle(Vector3 firstVec, Vector3 secondVec, bool normalize)
	{
		Vector3 H = firstVec + secondVec;

		if (normalize)
			return H.normalized;

		return H;
	}

	public static Vector3 DoVectorSorting(Vector3[] vectors) //Vector sorting algorithm for side independance, always return largest positive.
	{
		Vector3 largestVector = Vector3.zero;
		float largestSum = 0;

		try
		{
			for(int i = 0; i < vectors.Length; i++)
			{
				Vector3 vector = vectors[i];
				float sum = vector.x + vector.y + vector.z;
	
				if (sum > largestSum)
				{
					largestVector = vector;
					largestSum = sum;
				}
			}
		}
		catch (Exception e)
		{
			//CrashHelp.Report("MathHelp", e.Message, e);
		}
		return largestVector;
	}
	
	public static float RemapRange(float inputValue, float from1, float to1, float from2, float to2) 
	{
		return (inputValue - from1) / (to1 - from1) * (to2 - from2) + from2;
	}

	public static Vector3 AverageVector3(Vector3[] vectors)
	{
		Vector3 averagedVector = Vector3.zero;

		for (int i = 0; i < vectors.Length; i++)
			averagedVector += vectors[i];

		return averagedVector / vectors.Length;
	}

	public static Vector3 ReflectVector3(Vector3 normal, Vector3 inputVector)
	{
		float ratio = Vector3.Dot(normal, inputVector);
		return ((ratio * normal)*2f - inputVector).normalized;
	}

	public static Vector3[] IncrementArrayChannel(Vector3[] array, Vector3 axis, float amount)
	{
		Vector3[] newArray = new Vector3[array.Length];

		try
		{
			for (int i = 0; i < array.Length; i++)
			{
				Vector3 arrayItem = array[i];
				arrayItem.x = arrayItem.x + (amount * axis.x);
				arrayItem.y = arrayItem.y + (amount * axis.y);
				arrayItem.z = arrayItem.z + (amount * axis.z);
				newArray[i] = arrayItem;
			}
		}
		catch (Exception e)
		{
			//CrashHelp.Report("MathHelp", e.Message, e);
		}

		return newArray;
	}

	public static bool Vector3Equals(Vector3 a, Vector3 b, float epsilon)
	{
		return Vector3.SqrMagnitude(a - b) > epsilon;
	}

	public static float CalculateActorPolarAngle(Transform firstActor, Transform secondActor, bool normalize, Vector2? clamp)
	{
		float angle = 0f;
		
		try
		{
			if (firstActor != null && secondActor != null)
			{
				Vector3 actorDirection = secondActor.position - firstActor.position;
				Vector3 actorForward = firstActor.forward;

				angle = Vector3.Angle(actorForward, actorDirection);
				Vector3 side = Vector3.Cross(actorForward, actorDirection);
				
				if (side.y < 0) 
					angle = -angle;
			}
		}
		catch (Exception e)
		{
			//CrashHelp.Report("MathHelp", e.Message, e);
		}

		if (clamp.HasValue && normalize)
		{
			angle = Mathf.Clamp(angle, clamp.Value.x, clamp.Value.y);
			return angle /= Mathf.Abs(clamp.Value.x);
		}
		else
			return angle;
	}

	public static float FindHorizontalDirection(Transform start, Transform stop)
	{
		Vector3 actorDirection = (stop.position - start.position).normalized;

		Vector3 forward = start.forward;
		Vector3 right = start.right;
		Vector3 axis = Vector3.Cross(forward, right);
		
		//Rotate direction vector counter-clockwise, if the vector is to the left it will get a negative sign. Very useful formula.
		Vector3 rotatedVector = Quaternion.AngleAxis(-90f, axis) * actorDirection;
		
		float side = Vector3.Dot(forward, rotatedVector);
		
		return side;
	}

	public static float FindVerticalDirection(Transform start, Transform stop, Vector3 forwardDirection)
	{
		Vector3 vActorDirection = (stop.position - start.position);
		vActorDirection.y = 0f;
		vActorDirection = vActorDirection.normalized;
		Vector3 vActorReferenceDirection = (stop.position - start.position).normalized;
		Vector3 vActorRotationAxis = Vector3.Cross(vActorDirection, vActorReferenceDirection).normalized;
		Vector3 rotatedVector = Quaternion.AngleAxis(-90f, vActorRotationAxis) * vActorReferenceDirection;
		float side = Vector3.Dot(vActorDirection, rotatedVector);
		
//		Debug.DrawRay(start.position, vActorDirection, Color.red, 3f);
//		Debug.DrawRay(start.position, vActorReferenceDirection, Color.blue, 3f);
//		Debug.DrawRay(start.position, vActorRotationAxis, Color.black, 3f);
//		Debug.DrawRay(start.position, rotatedVector, Color.green, 3f);

		//Above or below?
		if (vActorReferenceDirection.y < 0f)
			side = -side;

		return side;
	}

	public static Vector3? RayIntersect(Vector3 rayStart, Vector3 finalPosition)
	{
		try
		{
			Vector3 projectionDir = finalPosition - rayStart;
			float projectionLength = projectionDir.magnitude;
			RaycastHit hit;

			#if UNITY_EDITOR
			Debug.DrawRay(rayStart, projectionDir, Color.black, 3f);
			#endif
			
			if (Physics.Raycast(rayStart, projectionDir, out hit, projectionLength))
				return hit.point;
		}
		catch (Exception e)
		{
			//CrashHelp.Report("MathHelp", e.Message, e);
		}
		
		return null;
	}

	public static Quaternion AngleAxisToQuaternion(Matrix4x4 matrix)
	{
		var qw = Mathf.Sqrt(1f + matrix.m00 + matrix.m11 + matrix.m22) / 2;
		var w = 4 * qw;
		var qx = (matrix.m21 - matrix.m12) / w;
		var qy = (matrix.m02 - matrix.m20) / w;
		var qz = (matrix.m10 - matrix.m01) / w;
		
		return new Quaternion(qx, qy, qz, qw);
	}

	public static bool IsQuaternionNone(Quaternion q) 
	{
		return float.IsNaN(q.x) || float.IsNaN(q.y) || float.IsNaN(q.z) || float.IsNaN(q.w);
	}
}
