using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;


/*
	Track - holds all data for one parameter track
*/

public class Track
{
	const int DEFAULT_DURATION = 100;
	const int MAX_SEGLIST_LENGTH = 10;
	const float VERYSMALL = 1e-8f;
	
	List<LipSyncData.Point> a = new List<LipSyncData.Point>();
	
	public void Clear()
	{
		seg.Clear();
		outpts.Clear();
	}
	
	List<Segment> seg = new List<Segment>();
	List<LipSyncData.Point> outpts = new List<LipSyncData.Point>();
	
	LipSyncData.Point lastp = new LipSyncData.Point(0, 0);
	int lastp_set = -1;
	public LipSyncData.Parameter par = new LipSyncData.Parameter();
	
	public int AddPhone(string phone, long t0)
	{
		if (seg.Count > 0)
		{
			/* adjust duration of the last segment in the list */
			seg[seg.Count-1].dur = t0 - seg[seg.Count-1].t0;
		}
		
		/* initialize new segment */
		Segment newseg = new Segment();
		newseg.id = phone;
		newseg.t0 = t0;
		newseg.dur = DEFAULT_DURATION;
		
		if (par.defs.ContainsKey(phone))
		{
			newseg.targets = par.defs[phone].targets;
			
			/* place new segment in the list */
			seg.Add(newseg);
		}
		return 0;
	}
	
	public void PushPointsToVector()
	{
		/* for each segment in the list, add it's targets to a */
		foreach (Segment s in seg)
			s.PushPointsToVector(this.a);
	}
	
	public float GetValue(long t)
	{
		/* now get the spline point */
		if (a.Count > 0)
			return Spline.GetPoint(this.a, t);
		else
			return 0.0f;
	}
	
	public float GetFilteredValue(long t)
	{
		float vout;
		float vin = GetValue(t);
		
		if (lastp_set == 0)
			vout = vin;	
		
		else if (t-lastp.t == 0)
			vout = lastp.v;	
		
		else
		{
			float k = Mathf.Exp(-(float)(t - lastp.t) / par.maxvel);
			vout = k * lastp.v + (1 - k) * vin;
			
			if (vout < VERYSMALL)
				vout = 0.0f;	
			
		}
		
		lastp = new LipSyncData.Point(t, vout);
		lastp_set = 1;
		
		return vout;
	}
}