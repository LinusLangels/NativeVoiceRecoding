using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;


/*
	Segment - holds instantiated target values (from PhoneDefine), start time and duration for a segment in the track
*/

public class Segment
{
	readonly int defaultDuration = 100;
	const int maxNumSegments = 10;
	
	public Dictionary<float, float> targets = new Dictionary<float, float>();
	public string id;
	public long t0;
	public long dur;
	
	/* 
	convert segment's target specification to points and add these to vector *pts 
	return numer of points added
	*/
	public int PushPointsToVector(List<LipSyncData.Point> pts)
	{
		/*
	    # temporal spacing of points. times are ignored, except for "gestures"
	    # instead we do this:
	    
	    # single point: if segment is longer than 2*tao, put point at start+tao and 
	    #               end-tao, else put point in middle of segment
	    
	    # multiple points: space points evenly, first point at start+10%, 
	    #               last at end-10%
	  	*/
		
		if (this.targets.Count == 0)
			return 0;
		
		else if (this.targets.Count == 1)
		{
			float v = this.targets.Values.ToList()[0];
			if (this.dur > this.defaultDuration)
			{
				pts.Add(new LipSyncData.Point(this.t0 + this.defaultDuration/2, v));	
				pts.Add(new LipSyncData.Point(this.t0 + this.dur - this.defaultDuration/2, v));
				return 2;
			}
			else
			{
				pts.Add(new LipSyncData.Point(this.t0 + this.dur / 2, v));
				return 1;
			}
		}
		else
		{
			for (int i = 0; i < this.targets.Count; ++i)
			{
				long t = this.t0 + (long)(0.1 * this.dur + 0.8 * this.dur * i / (this.targets.Count - 1));
				float v = this.targets.Values.ToList()[i];
				pts.Add(new LipSyncData.Point(t, v));
			}
			return this.targets.Count;
		}
	}
}
