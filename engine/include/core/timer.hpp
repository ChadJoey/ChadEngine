#pragma once
namespace ChadEngine {
	struct Timer
	{

		Timer() = default;
		Timer(float Duration) : Duration(Duration) {}

		bool Tick(float DeltaTime)
		{
			if (bFinished) return true;

			Elapsed += DeltaTime;
			if (Elapsed >= Duration)
			{
				Elapsed = Duration;
				bFinished = true;
				return true;
			}
			return false;
		}

		void Reset()
		{
			Elapsed = 0.f;
			bFinished = false;
		}

		void Restart(float NewDuration = -1.f)
		{
			if (NewDuration > 0.f) Duration = NewDuration;
			Reset();
		}

		// 0–1 normalized progress
		float Progress() const
		{
			return Duration > 0.f ? FMath::Clamp(Elapsed / Duration, 0.f, 1.f) : 1.f;
		}

		bool  IsFinished()  const { return bFinished; }
		float GetElapsed()  const { return Elapsed; }
		float GetDuration() const { return Duration; }
		float GetRemaining() const { return FMath::Max(0.f, Duration - Elapsed); }

	private:
		float Duration = 1.f;
		float Elapsed = 0.f;
		bool  bFinished = false;
	};
}
