using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;

public static class TransformHelp
{
	public static Transform FindMainParent(Transform parent)
	{
		while (parent.parent)
			parent = parent.parent;
		
		return parent;
	}

	/// <summary>
	/// Finds a child in the same mesh as the assigned parent
	/// </summary>
	public static Transform FindChildRecursive(Transform parent, string name)
	{
		try
		{
			if (string.Compare(parent.name, name, true) == 0)
				return parent;
			else
			{
				foreach (Transform child in parent)
				{
					Transform foundChild = FindChildRecursive(child, name);
					if (foundChild)
						return foundChild;
				}
			}
		}
		catch (Exception e)
		{
			//CrashHelp.Report("TextureScale", e.Message, e);
		}
		return null;
	}

	public static void FindChildrenRecursive(Transform transform, string[] validNames, Dictionary<string, Transform> mappedTransforms)
	{
		try
		{
			for (int i = 0; i < validNames.Length; i++)
			{
				string currentName = validNames[i];

				if (string.Compare(transform.name, currentName, true) == 0)
				{
					if (!mappedTransforms.ContainsKey(currentName))
					{
						mappedTransforms.Add(currentName, transform);
					}
				}
			}

			for (int i = 0; i < transform.childCount; i++)
			{
				FindChildrenRecursive(transform.GetChild(i), validNames, mappedTransforms);
			}
		}
		catch (Exception e)
		{
			//CrashHelp.Report("TextureScale", e.Message, e);
		}
	}

	public static T FindComponentRecursive<T>(Transform root, string name = "") where T : UnityEngine.Component
	{
		if (root.GetComponent<T>() != null)
		{
			if (string.IsNullOrEmpty(name))
				return root.GetComponent<T>();
			else if (root.name.Contains(name))
				return root.GetComponent<T>();
		}

		else
		{
			foreach (Transform child in root)
			{
				T component = FindComponentRecursive<T>(child, name);
				if (component)
					return component;
			}
		}

		return default(T);
	}
}
