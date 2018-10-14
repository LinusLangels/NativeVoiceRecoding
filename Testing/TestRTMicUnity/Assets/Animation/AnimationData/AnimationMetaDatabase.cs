using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

public class AnimationMetaDatabase : MonoBehaviour 
{
	public static readonly int MaxSearchDepth = 2;

	private int searchDepth = 0;

	public static AnimationMetaDatabase Instance { get; private set; }
	
	[SerializeField]
	public List<MetaInfoItem> dialogueDatabase;
	[SerializeField]
	public List<MetaInfoItem> listenDatabase;
	[SerializeField]
	public List<MetaInfoItem> idleDatabase;
	[SerializeField]
	public List<MetaInfoItem> actionDatabase;
	[SerializeField]
	public List<MetaInfoItem> ccDatabase;

	public Dictionary<string, List<MetaInfoItem>> databases;
	
	void Awake()
	{
		Instance = this;
		InitializeDatabases();
	}

	//Allow user of this method to optionally pass in a stance for a more detailed lookup.
	public MetaInfoItem GetMeta(string type, string actionID, string stance)
	{
		try
		{
			if (this.databases.ContainsKey(type))
			{
				actionID = actionID.ToLower();
				string currentStance = stance;
				List<MetaInfoItem> list = this.databases[type];
				MetaInfoItem item = null;
				bool itemExists = false;

				this.searchDepth += 1;

				for (int i = 0; i < list.Count; i++)
				{
					MetaInfoItem currentItem = list[i];

					if (currentItem.ID == actionID)
					{
						itemExists = true;

						if (currentItem.Stance == stance)
						{
							item = currentItem;
							break;
						}
					}
				}

				if (item == null && itemExists && this.searchDepth <= MaxSearchDepth)
				{
					//If first iteration fails we use lookup based on stance.
					if (searchDepth == 1)
					{
						if (currentStance.Contains("Sitting"))
						{
							item = GetMeta(type, actionID, "SittingNormal");
						}
						
						if (currentStance.Contains("Standing"))
						{
							item = GetMeta(type, actionID, "StandingNormal");
						}
					}
					//Else we default to standing stance.
					else
					{
						item = GetMeta(type, actionID, "StandingNormal");
					}
				}

				this.searchDepth = 0;
				
				return item;
			}
		}
		catch (Exception e)
		{
			//CrashHelp.Report("AnimationMetaDatabase", e.Message, e);
		}

		return null;
	}
	
	public void Initialize()
	{
		dialogueDatabase = new List<MetaInfoItem>();
		listenDatabase = new List<MetaInfoItem>();
		idleDatabase = new List<MetaInfoItem>();
		actionDatabase = new List<MetaInfoItem>();
		ccDatabase = new List<MetaInfoItem>();

		InitializeDatabases();
	}

	void InitializeDatabases()
	{
		databases = new Dictionary<string, List<MetaInfoItem>>();

		//Interaction
		databases.Add("action", actionDatabase);

		//Accessors to get the dialogue database
		databases.Add("dialogue", dialogueDatabase);
		databases.Add("listen", listenDatabase);
		databases.Add("sound", dialogueDatabase);

		//Accessors to get the idle database
		databases.Add("idle", idleDatabase);
		databases.Add("scene", idleDatabase);
		databases.Add("music", idleDatabase);

		//Accessors to get the cc database
		databases.Add("cc", ccDatabase);
	}
}

[System.Serializable] 
public class MetaInfoItem
{
	public string ID;
	public string Name;
	public string Text;

	public bool Enabled;
	public AnimationType Type;

	public int StartFrame;
	public int EndFrame;

	public int EntryFrame;
	public int ExitFrame;

	public int BlendInFrames;
	public int BlendOutFrames;

	public int PreviewFrame;
	public int SilenceFrames;

	public float MoodMultiplier;
	public float MoodIntensity;

	public string Attitude;
	public string Stance;

	public string TargetState;

	public AudioClip Sound;

	public MetaInfoItem() { }
}
