using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;

public static class ParMap
{
	public static Dictionary<int, List<float>> FillMap()
	{
		Dictionary<int, List<float>> parMap = new Dictionary<int, List<float>>();
		
		parMap[0] = new List<float>();
		parMap[0].Add(-1.8901f);
		parMap[0].Add(-2.1658f);
		parMap[0].Add(1.85f);
		parMap[0].Add(1.6504f);
		
		parMap[1] = new List<float>();
		parMap[1].Add(1.6505f);
		parMap[1].Add(0.24509f);
		parMap[1].Add(-2.1893f);
		parMap[1].Add(-1.956f);
		
		parMap[2] = new List<float>();
		parMap[2].Add(0f);
		parMap[2].Add(0f);
		parMap[2].Add(0f);
		parMap[2].Add(0f);
		
		parMap[3] = new List<float>();
		parMap[3].Add(-0.032879f);
		parMap[3].Add(0.15945f);
		parMap[3].Add(0.93316f);
		parMap[3].Add(-0.37301f);
		
		parMap[4] = new List<float>();
		parMap[4].Add(-0.013689f);
		parMap[4].Add(4.5524f);
		parMap[4].Add(0.30064f);
		parMap[4].Add(-0.070321f);
		
		parMap[5] = new List<float>();
		parMap[5].Add(-0.46381f);
		parMap[5].Add(-0.078773f);
		parMap[5].Add(0.74313f);
		parMap[5].Add(0.60975f);
		
		parMap[6] = new List<float>();
		parMap[6].Add(0.072868f);
		parMap[6].Add(-0.047956f);
		parMap[6].Add(-0.041132f);
		parMap[6].Add(-0.086893f);
		
		parMap[7] = new List<float>();
		parMap[7].Add(0.19657f);
		parMap[7].Add(0.30455f);
		parMap[7].Add(-0.23306f);
		parMap[7].Add(-0.22737f);
		
		parMap[8] = new List<float>();
		parMap[8].Add(1.5075f);
		parMap[8].Add(1.8619f);
		parMap[8].Add(-1.273f);
		parMap[8].Add(-1.1031f);
		
		parMap[9] = new List<float>();
		parMap[9].Add(0.11133f);
		parMap[9].Add(1.1985f);
		parMap[9].Add(-0.52328f);
		parMap[9].Add(-0.085067f);
		
		return parMap;
	}
}
