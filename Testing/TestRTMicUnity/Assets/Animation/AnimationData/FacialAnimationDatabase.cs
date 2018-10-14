using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

public class FacialAnimationDatabase : MonoBehaviour 
{
	public static readonly int MaxSearchDepth = 2;

	public static FacialAnimationDatabase Instance { get; private set; }
	
	[SerializeField]
	public List<FacialAnimationData> dialogueDatabase;

	[SerializeField]
	public List<FacialAnimationData> idleDatabase;

	[SerializeField]
	public List<FacialAnimationData> ccDatabase;

	[SerializeField]
	public List<FacialAnimationData> actionDatabase;

	public Dictionary<string, List<FacialAnimationData>> databases;

	//Allow user of this method to optionally pass in a stance for a more detailed lookup.
	public FacialAnimationData GetFacialAnimation(string type, string actionID, InteractionAnimationType interactionType, Stance stance)
	{
		try
		{
			if (this.databases.ContainsKey(type))
			{
				actionID = actionID.ToLower();
				List<FacialAnimationData> list = this.databases[type];
				FacialAnimationData item = null;

				//Track how many recursive runs we have done, so we dont run into stack overflows.
				this.searchDepth += 1;

				for (int i = 0; i < list.Count; i++)
				{
					FacialAnimationData currentItem = list[i];

					if (string.Compare(currentItem.ID, actionID, true) == 0 && currentItem.Stance == (int)stance && currentItem.InteractionType == interactionType)
					{
						item = currentItem;
						break;
					}
				}

				//We did not find a excakt match for the facial animation we where looking for, try to find an approximate hit.
				if (item == null && this.searchDepth <= MaxSearchDepth)
				{
					//If first iteration fails we use lookup based on stance.
					if (searchDepth == 1)
					{
						if ((stance & Stance.UseStanding) != 0)
						{
							item = GetFacialAnimation(type, actionID, interactionType, Stance.StandingNormal);
						}

						if ((stance & Stance.UseSitting) != 0)
						{
							item = GetFacialAnimation(type, actionID, interactionType, Stance.SittingNormal);
						}
					}
					//Else we default to standing stance.
					else
					{
						item = GetFacialAnimation(type, actionID, interactionType, Stance.StandingNormal);
					}
				}

				//Reset for the next caller.
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

	private int searchDepth = 0;
	
	void Awake()
	{
		Instance = this;
		searchDepth = 0;
		InitializeDatabases();
	}
	
	public void Initialize()
	{
		dialogueDatabase = new List<FacialAnimationData>();
		idleDatabase = new List<FacialAnimationData>();
		ccDatabase = new List<FacialAnimationData>();
		actionDatabase = new List<FacialAnimationData>();
		
		InitializeDatabases();
	}

	void InitializeDatabases()
	{
		databases = new Dictionary<string, List<FacialAnimationData>>();
		
		//Interaction
		databases.Add("action", actionDatabase);
		
		//Accessors to get the dialogue database
		databases.Add("dialogue", dialogueDatabase);
		databases.Add("listen", dialogueDatabase);
		databases.Add("sound", dialogueDatabase);
		
		//Accessors to get the idle database
		databases.Add("idle", idleDatabase);
		databases.Add("scene", idleDatabase);
		databases.Add("music", idleDatabase);

		//Character Creator.
		databases.Add("Creator", ccDatabase);
	}
}

[Serializable]
public class FacialAnimationData
{
	public string ID;
	public AnimationType Type;
	public int Stance;
	public int Length;
	public DialogueAnimationType DialogType;
	public InteractionAnimationType InteractionType;
	public List<RotationFacialAnimationChannel> RotationChannels;
	public List<MorphFacialAnimationChannel> MorphChannels;

	public FacialAnimationData() { }
}

[Serializable]
public class RotationFacialAnimationChannel
{
	public string ChannelName;
	public bool CanMirror;
	public bool CanLoop;
	public List<Vector3> Keys;

    public RotationFacialAnimationChannel Copy()
    {
        RotationFacialAnimationChannel copy = new RotationFacialAnimationChannel();
        copy.ChannelName = this.ChannelName;
        copy.CanMirror = this.CanMirror;
        copy.CanLoop = this.CanLoop;

        List<Vector3> keysCopy = new List<Vector3>(this.Keys.Count);

        for (int i = 0; i < this.Keys.Count; i++)
        {
            keysCopy.Add(this.Keys[i]);
        }

        copy.Keys = keysCopy;

        return copy;
    }
}

[Serializable]
public class MorphFacialAnimationChannel
{
	public string ChannelName;
	public bool CanMirror;
	public bool CanLoop;
	public List<float> Keys;

    public MorphFacialAnimationChannel Copy()
    {
        MorphFacialAnimationChannel copy = new MorphFacialAnimationChannel();
        copy.ChannelName = this.ChannelName;
        copy.CanMirror = this.CanMirror;
        copy.CanLoop = this.CanLoop;

        List<float> keysCopy = new List<float>(this.Keys.Count);

        for (int i = 0; i < this.Keys.Count; i++)
        {
            keysCopy.Add(this.Keys[i]);
        }

        copy.Keys = keysCopy;

        return copy;
    }
}
