using UnityEngine;
using UnityEditor;
using System.Collections;
using System.Collections.Generic;
using Newtonsoft.Json.Linq;
using Newtonsoft.Json;

public class VisemeVisualizer : EditorWindow 
{
	public class ChannelData
	{
		public Color DisplayColor;
		public Vector3 Offset;
		public float Scale;

		public ChannelData(Color displayColor, Vector3 offset, float scale)
		{
			this.DisplayColor = displayColor;
			this.Offset = offset;
			this.Scale = scale;
		}
	}

	public class AnimationChannel
	{
		public bool ShouldDraw;
		public Color DisplayColor;
		public Vector3 Offset;
		public float Scale;
		public string ChannelName;
		public List<float> KeyFrames;

		public AnimationChannel(string name)
		{
			if (channelMetaData.ContainsKey(name))
			{
				this.DisplayColor = channelMetaData[name].DisplayColor;
				this.Offset = channelMetaData[name].Offset;
				this.Scale = channelMetaData[name].Scale;
			}
			else
			{
				this.DisplayColor = Color.white;
				this.Offset = Vector3.zero;
				this.Scale = 1f;
			}

			this.ChannelName = name;
			this.KeyFrames = new List<float>();
			this.ShouldDraw = true;
		}

		public void AppendData(float[] animationData)
		{
			this.KeyFrames.AddRange (animationData);
		}
	}

	private static Dictionary<string, ChannelData> channelMetaData;
	public static Vector2 scrollPos;
	public static VisemeVisualizer window;
	public static Vector2 windowSize = new Vector2 (1000, 600);
	public static float TIME_SPACING;

	private Dictionary<string, AnimationChannel> animationChannels;
	private float greatestViewWidth;

	[MenuItem("Plotagon/VisemeVisualizer")]
	public static void Init()
	{
		CreateChannelMetaData ();

		window = EditorWindow.GetWindow<VisemeVisualizer> ();
		window.title = "Viseme Visualizer";
		window.maxSize = windowSize;
		window.maxSize = windowSize * 0.8f;

		window.Show ();
		window.Focus ();

		TIME_SPACING = windowSize.x / 100f;
	}

	static void CreateChannelMetaData()
	{
		channelMetaData = new Dictionary<string, ChannelData>();

		channelMetaData.Add ("Mid_Head_Jnt_03", new ChannelData(Color.red, new Vector3(0f, -150, 0f), 10f));
		channelMetaData.Add ("Corner_In", new ChannelData(Color.blue, new Vector3(0f, -100f, 0f), 0.3f));
		channelMetaData.Add ("I", new ChannelData(Color.green, new Vector3(0f, -50f, 0f), 0.3f));
		channelMetaData.Add ("Lip_LowerUp", new ChannelData(Color.yellow, new Vector3(0f, 0f, 0.3f), 1f));
		channelMetaData.Add ("Lip_LowerDown", new ChannelData(Color.white, new Vector3(0f, 50f, 0.3f), 1f));
		channelMetaData.Add ("Lip_UpperUp", new ChannelData(Color.magenta, new Vector3(0f, 100f, 0.3f), 1f));
		channelMetaData.Add ("Lip_UpperDown", new ChannelData(Color.black, new Vector3(0f, 150f, 0.3f), 1f));
	}

	void OnEnable()
	{
		this.animationChannels = new Dictionary<string, AnimationChannel> ();
		this.animationChannels.Clear ();

		this.greatestViewWidth = windowSize.x;
	}

	void OnDisable()
	{
		this.animationChannels.Clear ();
		Debug.Log ("disabled window");
	}

	void OnGUI()
	{
		Event e = Event.current;

		if (e.type == EventType.ExecuteCommand) 
		{
			JObject animationData = JObject.Parse(e.commandName);

			foreach (JProperty property in animationData.Properties())
			{
				if (this.animationChannels.ContainsKey(property.Name))
				{
					float[] keys = (property.Value as JArray).ToObject<float[]>();

					this.animationChannels[property.Name].AppendData(keys);
				}
				else
				{
					this.animationChannels.Add(property.Name, new AnimationChannel(property.Name));

					float[] keys = (property.Value as JArray).ToObject<float[]>();
					
					this.animationChannels[property.Name].AppendData(keys);
				}
			}

			e.Use();
		}

		DrawToggleGroup ();
		DrawAnimationCurves ();
	}

	void DrawToggleGroup()
	{
	}

	void DrawAnimationCurves()
	{
		if (animationChannels == null)
			return;

		scrollPos = GUI.BeginScrollView (new Rect (0, 0, position.width, position.height), scrollPos, new Rect (0, 0, this.greatestViewWidth+20, position.height),true, false);

		foreach (KeyValuePair<string, AnimationChannel> channel in animationChannels) 
		{
			if (channel.Value.ShouldDraw)
			{
				List<float> keys = channel.Value.KeyFrames;
				float time = 0.0f;
				
				for (int i = 0; i < keys.Count-1; i++)
				{
					float val1 =  (keys[i] * channel.Value.Scale) + (window.position.height /2f) + channel.Value.Offset.y;
					float val2 = (keys[i+1] * channel.Value.Scale) + (window.position.height /2f) + channel.Value.Offset.y;
					
					Vector3 startPos = new Vector3(time, val1, 0f);
					
					time += TIME_SPACING;
					
					Vector3 endPos = new Vector3(time, val2, 0f);
					
					Color defaultColor = Handles.color;
					
					Handles.color = channel.Value.DisplayColor;
					
					Handles.DrawLine(startPos, endPos);
					
					Handles.color = defaultColor;

					if (time >= this.greatestViewWidth)
					{
						this.greatestViewWidth = time;
					}
				}
			}
		}

		GUI.EndScrollView ();
	}
}
