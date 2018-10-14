using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Text;

public class TextAnalysisTools 
{
	public static int CountWords(string input)
	{
		int c = 0;
		for (int i = 1; i < input.Length; i++)
		{
			if (char.IsWhiteSpace(input[i - 1]) == true)
			{
				if (char.IsLetterOrDigit(input[i]) == true ||
				    char.IsPunctuation(input[i]))
				{
					c++;
				}
			}
		}
		if (input.Length > 2)
		{
			c++;
		}
		return c;
	}

	static int CountSyllablesInWord(string word)
	{
		char[] vowels = { 'a', 'e', 'i', 'o', 'u', 'y' };
		string currentWord = word;
		int numVowels = 0;
		bool lastWasVowel = false;
		foreach (char wc in currentWord)
		{
			bool foundVowel = false;
			foreach (char v in vowels)
			{
				//don't count diphthongs
				if (v == wc && lastWasVowel)
				{
					foundVowel = true;
					lastWasVowel = true;
					break;
				}
				else if (v == wc && !lastWasVowel)
				{
					numVowels++;
					foundVowel = true;
					lastWasVowel = true;
					break;
				}
			}
			
			//if full cycle and no vowel found, set lastWasVowel to false;
			if (!foundVowel)
				lastWasVowel = false;
		}
		//remove es, it's _usually? silent
		if (currentWord.Length > 2 && 
		    currentWord.Substring(currentWord.Length - 2) == "es")
			numVowels--;
		// remove silent e
		else if (currentWord.Length > 1 &&
		         currentWord.Substring(currentWord.Length - 1) == "e")
			numVowels--;
		
		return numVowels;
	}

	//Split input text in individual words.
	static IEnumerable<string> NextWord(string text)
	{
		StringBuilder builder = new StringBuilder();

		for (int i = 0; i < text.Length; i++)
		{
			//This will return the last word in the text.
			if (i >= (text.Length-1))
			{
				yield return builder.ToString();
				builder.Length = 0;
			}

			//Return and reset the word string eachtime we encounter a whitespace.
			if (char.IsWhiteSpace(text[i]))
			{
				yield return builder.ToString();
				builder.Length = 0;
			}

			if (!char.IsWhiteSpace(text[i]) && char.IsLetterOrDigit(text[i]))
				builder.Append(text[i]);
		}
	}

	public static int CountSyllables(string text)
	{
		int syllableCount = 0;

		foreach(string word in NextWord(text))
			syllableCount += CountSyllablesInWord(word);

		return syllableCount;
	}

	public static float EsimateTextLength(string text, int syllablesPerMinute)
	{
		//Always padd with one extra syllable, account for short weird sentences.
		int totalSyllables = CountSyllables(text) + 1;
		float syllablesPerSecond = syllablesPerMinute / 60f;
		float totalReadingTimeSeconds = totalSyllables / syllablesPerSecond;

		return totalReadingTimeSeconds;
	}
}
