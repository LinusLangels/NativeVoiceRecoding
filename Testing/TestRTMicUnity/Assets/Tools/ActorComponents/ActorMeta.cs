using UnityEngine;
using System;
using System.Collections;

/// <Summary>
/// Actor Meta 
/// 
/// Description: This script is attached to Actor Content when exported
/// and collects useful information that we can use without having to load the actual game objects.
/// 
/// All meta files is loaded when the application is started.
/// </Summary>
public class ActorMeta : MonoBehaviour 
{
	public string Data;
	
	public Texture2D Icon;
}

