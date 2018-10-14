using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;

public class Spline
{
	public static float GetPoint(List<LipSyncData.Point> list, float t)
	{
		if (list.Count == 0)
			return 0.0f;
		
		LipSyncData.Point first = list[0];
		
	 	// make a local copy of the elements to vector a
	  	// first check if we need to add elements at the beginning
		if (t <= first.t)
			return first.v;
		
		// copy rest of the elements
		List<LipSyncData.Point> a = new List<LipSyncData.Point>(list);
		
		if (list.Count > 1 && t < list[1].t )
			a.Insert(0, new LipSyncData.Point(first.t - 1, first.v));	
		
		// find interval where t fits
		int ii = 0;
		for (int j = 0; j < a.Count; j++)
		{
			if ( a[j].t > t)
			{
				ii = j;
				break;
			}
			
			// we're off the end, return last value
			if (j == a.Count - 1)
				return a[a.Count - 1].v;
		}

		// one before the end, add one point 
		if (ii == a.Count - 1)  
			a.Add(new LipSyncData.Point(a[a.Count - 1].t + 1, a[a.Count - 1].v));	
		
		LipSyncData.Point p = a[ii];
		
		if (a[ii - 1].t == p.t)
			return 0.5f * (a[ii - 1].v * p.v);
		
		
		float x = (float)(t - a[ii - 1].t) / (p.t - a[ii - 1].t);
		
		t = 0.0f;
		for(int j = ii - 2; j < ii + 2; j++)
			t += a[j].v * blendfunc_cmr(j - ii, x);
		
		return t;
	}
	
	static float blendfunc_cmr(int i, float x)
	{
		switch (i) 
		{
		  case -2: 
			return ((-x + 2) * x - 1)* x / 2.0f;
		  case -1: 
			return (((3 * x - 5) * x)* x + 2) / 2.0f;
		  case 0 : 
			return ((-3 * x + 4) * x + 1) * x / 2.0f;
		  case 1 : 
			return ( x - 1) * x * x / 2.0f;
		  default: 
			return 0;	  
		}
	}
}
