using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using Plotagon.Serialization;

public class PhonemeBuffer
{
	public class InternalBuffer
	{
		string UniqueID;
		List<PhonemeContainer> buffer;
		float bufferLength;
		float currentBufferLength;
		float firstStartTime;
		float lastEndTime;
		InternalBuffer Next;

		public InternalBuffer(float bufferLength, int expectedPhonemeCount)
		{
			this.UniqueID = Guid.NewGuid().ToString();
			this.bufferLength = bufferLength;

			//Set to low values so that when first phoneme is added it will always have greater values.
			this.firstStartTime = -10f;
			this.lastEndTime = -10f;

			this.buffer = new List<PhonemeContainer>(expectedPhonemeCount);
		}

		public void SetNextBuffer(InternalBuffer nextBuffer)
		{
			this.Next = nextBuffer;
		}

		public InternalBuffer GetNextBuffer()
		{
			return this.Next;
		}

		//Check epsilon if needed
		public void SetDefaultValues(float startTime, float timeSpan, int numberOfPhonemes)
		{
			float timeStep = timeSpan / numberOfPhonemes;

			for (int i = 0; i < numberOfPhonemes; i++) 
			{
				PhonemeContainer silence = new PhonemeContainer();
				silence.Phoneme = "sil";
				silence.Start = startTime;
				silence.End = startTime + timeStep;
				startTime += timeStep;

				AddPhoneme(silence);
			}
		}

		public bool AddPhoneme(PhonemeContainer phoneme)
		{
			//First phoneme added?
			if (this.buffer.Count == 0) {
				this.firstStartTime = phoneme.Start;
			}

			//Find largest possible time within the buffer.
			if (phoneme.End >= this.lastEndTime) {
				this.lastEndTime = phoneme.End;
			}

			//how long is this buffer in local time.
			float timeRange = this.lastEndTime - this.firstStartTime;

			//All buffers track their own size.
			this.currentBufferLength = timeRange;

			if (timeRange >= this.bufferLength) 
			{
				this.buffer.Add (phoneme);
				return true;
			}

			this.buffer.Add (phoneme);
			return false;
		}

		public List<PhonemeContainer> GetBufferData()
		{
			return this.buffer;
		}

		public void Clear()
		{
			this.buffer.Clear ();
			this.firstStartTime = 0f;
			this.lastEndTime = 0f;
		}

		public int NumberOfItems()
		{
			return this.buffer.Count;
		}

		public string GetBufferID()
		{
			return this.UniqueID;
		}

		public float GetBufferLength()
		{
			return this.currentBufferLength;
		}

		public void TrimBufferSilence(bool padding, float paddingTime)
		{
			//assume total silence, aka first index.
			int stopIndex = 0;

			//Trim silence from end until we meet a phoneme.
			for (int i = this.buffer.Count-1; i >= 0; i--)
			{
				PhonemeContainer phoneme = this.buffer[i];

				if (string.CompareOrdinal(phoneme.Phoneme, "sil") != 0)
				{
					//Always use the index ahead of current.
					stopIndex = i;
					break;
				}
			}

			//New trimmed buffer will be stop index plus one because otherwise we add one to few becuase of zero based index.
			//Add another 4 phonemes of silence at the end as well.
			List<PhonemeContainer> trimmedBuffer = new List<PhonemeContainer>(stopIndex+1+4);

			if (stopIndex > 0)
			{
				for (int i = 0; i <= stopIndex; i++)
				{
					trimmedBuffer.Add(this.buffer[i]);
				}

				//Replace buffer with trimmed version.
				this.buffer = trimmedBuffer;

				//Update our range values.
				this.firstStartTime = this.buffer[0].Start;
				this.lastEndTime = this.buffer[this.buffer.Count-1].End;

				//Add silence phonemes at the end as a sort of padding
				if (padding)
				{
					//Padd from the last phoeneme endtime to ensure contineous time.
					//This routine also takes care of adjusting the current length of the buffer.
					SetDefaultValues(this.buffer[this.buffer.Count-1].End, paddingTime, 4);
				}
			}
			else
			{
				this.currentBufferLength = 0f;
				this.buffer = new List<PhonemeContainer>();
			}
		}
	}

	private List<InternalBuffer> phonemeBuffers;
	private InternalBuffer currentBuffer;
	private InternalBuffer completeBuffer;
	
	public Action<InternalBuffer> OnBufferChanged;
	public Action<InternalBuffer> OnFinished;

	public PhonemeBuffer(float bufferLength, int numberOfBuffers)
	{
		this.phonemeBuffers = new List<InternalBuffer>(numberOfBuffers);

		//Create buffers;
		for (int i = 0; i < numberOfBuffers; i++)
		{
			InternalBuffer buffer = new InternalBuffer(bufferLength, 4);
			phonemeBuffers.Add(buffer);
		}

		//First buffer added get lowest possible starttime. Then we work ourself up to 0 from there.
		int timeScale = -numberOfBuffers;

		//Create cyclic buffer pointers.
		for (int i = 0; i < numberOfBuffers; i++)
		{
			InternalBuffer buffer = phonemeBuffers[i];

			//loop back to first index when at the end.
			buffer.SetNextBuffer(phonemeBuffers[i < numberOfBuffers-1 ? i+1 : 0]);

			//The time at which this buffer will start.
			float startTime = timeScale * bufferLength;

			//All buffers are filled with silent phonemes.
			buffer.SetDefaultValues(startTime, bufferLength, 4);

			//Make sure all buffers have different default times stretching back.
			timeScale += 1;
		}

		//Clear this index since its going to be our frontbuffer.
		this.phonemeBuffers[0].Clear();
		this.currentBuffer = this.phonemeBuffers[0];

		//This buffer will eat all incoming phonemes even when it gets full.
		this.completeBuffer = new InternalBuffer (bufferLength, 100);
	}

	void SwapBuffers()
	{
		//Put into temporary variable so we dont loose the reference when we repoint.
		InternalBuffer temp = this.currentBuffer;

		//This make our current buffers next pointer become our front buffer, this will cycle infinite.
		this.currentBuffer = temp.GetNextBuffer();

		//Always clear frontbuffer when it becomes active.
		this.currentBuffer.Clear();
	}

	public void AddPhoneme(PhonemeContainer phoneme)
	{
		//Debug.Log ("Unity: Phoneme is: " + phoneme.Phoneme + "\n with start: " + phoneme.Start + "\n with stop: " + phoneme.End);

		if (this.currentBuffer != null)
		{
			//This buffer tracks all phonemes added while recording. Do not touch!
			this.completeBuffer.AddPhoneme(phoneme);

			//Check if current buffer is full (timespan of phonemes is greater than bufferlength set when created)
			bool shouldSwap = this.currentBuffer.AddPhoneme(phoneme);

			if(shouldSwap)
			{
				//Debug.Log (this.CurrentBuffer.GetBufferID());

				if (this.OnBufferChanged != null)
				{
					//By sending the current buffer,
					//we allow the caller to get all the subsequent buffers from there using its next pointer.
					this.OnBufferChanged(this.currentBuffer);
				}

				SwapBuffers();
			}
		}
	}

	public void SubscribeBuffersChanged(Action<InternalBuffer> handler)
	{
		this.OnBufferChanged = handler;
	}

	public void SubscribeOnFinished(Action<InternalBuffer> handler)
	{
		this.OnFinished = handler;
	}

	public InternalBuffer Finalize()
	{
		//Remove unneccessary silence. And optionally add some padding of silence at the end.
		this.completeBuffer.TrimBufferSilence(true, 0.2f);

		//Call animation system with finished phoneme buffer.
		if (this.OnFinished != null)
			this.OnFinished(this.completeBuffer);
		
		this.OnBufferChanged = null;
		this.OnFinished = null;

		return this.completeBuffer;
	}
}