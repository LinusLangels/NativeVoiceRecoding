using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;

public static class SymMap
{
	public static Dictionary<string, string> FillMap() 
	{
		Dictionary<string, string> symMap = new Dictionary<string, string>();
		symMap.Add("@", "eh");
		symMap.Add("@0", "eh");
		symMap.Add("@1", "eh");
		symMap.Add("@2", "eh");
		
		symMap.Add("@@", "er");
		symMap.Add("@@0", "er");
		symMap.Add("@@1", "er");
		symMap.Add("@@2", "er");
		
		symMap.Add("ae", "ae");
		symMap.Add("a", "ae");
		symMap.Add("a0", "ae");
		symMap.Add("a1", "ae");
		symMap.Add("a2", "ae");
		
		symMap.Add("aa", "aq");
		symMap.Add("aa1", "aq");
		symMap.Add("aa2", "aq");
		
		symMap.Add("ay", "ay");
		symMap.Add("ai", "ay");
		symMap.Add("ai0", "ay");
		symMap.Add("ai1", "ay");
		symMap.Add("ai2", "ay");
		
		symMap.Add("au", "aw");
		symMap.Add("au0", "aw");
		symMap.Add("au1", "aw");
		symMap.Add("au2", "aw");
		
		symMap.Add("b", "B");
		
		symMap.Add("ch", "ch");
		
		symMap.Add("d", "D");
		
		symMap.Add("dh", "dh");
		
		symMap.Add("e", "eh");
		symMap.Add("e1", "eh");
		symMap.Add("e2", "eh");
		symMap.Add("e3", "eh");
		
		symMap.Add("e@", "ex");
		symMap.Add("e@0", "ex");
		symMap.Add("e@1", "ex");
		symMap.Add("e@2", "ex");
		
		symMap.Add("ei", "ey");
		symMap.Add("ei0", "ey");
		symMap.Add("ei1", "ey");
		symMap.Add("ei2", "ey");
		
		symMap.Add("f", "F");
		
		symMap.Add("g", "G");
		
		symMap.Add("hh", "hh");
		symMap.Add("h", "hh");
		
		symMap.Add("ih", "ih");
		symMap.Add("i", "ih");
		symMap.Add("i0", "ih");
		symMap.Add("i1", "ih");
		symMap.Add("i2", "ih");
		
		symMap.Add("i@", "ir");
		symMap.Add("i@0", "ir");
		symMap.Add("i@1", "ir");
		symMap.Add("i@2", "ir");
		
		symMap.Add("ii", "iy");
		symMap.Add("ii0", "iy");
		symMap.Add("ii1", "iy");
		symMap.Add("ii2", "iy");
		
		symMap.Add("jh", "jh");
		
		symMap.Add("k", "K");
		symMap.Add("l", "L");
		symMap.Add("m", "M");
		symMap.Add("n", "N");
		
		symMap.Add("nx", "nx");
		symMap.Add("ax", "ax");
		symMap.Add("ao", "ao");
		
		symMap.Add("o", "oh");
		symMap.Add("o0", "oh");
		symMap.Add("o1", "oh");
		symMap.Add("o2", "oh");
		
		symMap.Add("oi", "oy");
		symMap.Add("oi0", "oy");
		symMap.Add("oi1", "oy");
		symMap.Add("oi2", "oy");
		
		symMap.Add("oo", "ow");
		symMap.Add("oo0", "ow");
		symMap.Add("oo1", "ow");
		symMap.Add("oo2", "ow");
		
		symMap.Add("p", "P");
		symMap.Add("r", "R");
		symMap.Add("s", "S");
		symMap.Add("sh", "Sh");
		symMap.Add("t", "T");
		symMap.Add("th", "th");
		
		symMap.Add("u", "uh");
		symMap.Add("u0", "uh");
		symMap.Add("u1", "uh");
		symMap.Add("u2", "uh");
		
		symMap.Add("u@", "ur");
		symMap.Add("u@0", "ur");
		symMap.Add("u@1", "ur");
		symMap.Add("u@2", "ur");
		
		symMap.Add("uh", "ah");
		symMap.Add("uh0", "ah");
		symMap.Add("uh1", "ah");
		symMap.Add("uh2", "ah");
		
		symMap.Add("uu", "uw");
		symMap.Add("uu0", "uw");
		symMap.Add("uu1", "uw");
		symMap.Add("uu2", "uw");
		
		symMap.Add("v", "V");
		symMap.Add("w", "W");
		symMap.Add("y", "Y");
		symMap.Add("z", "Z");
		symMap.Add("zh", "jh");

		//Extra phoneme maps added after analyzing voice phoneme output.
		symMap.Add("E", "eh");
		symMap.Add("S", "S");
		symMap.Add("T", "T");
		symMap.Add("O", "oh");
		
		symMap.Add("R", "R");
		symMap.Add("rx", "R");
		symMap.Add("sil", ".");
		symMap.Add("_", ".");
		
		return symMap;
	}
}
