#pragma once
#include <string>

enum message_type
{
	Yes,
	No,
	CanWeMeet,
	ImBusy,
	YesWhatTimeWeMeet,
	MeetingTime,
	OkIWillComeAtThisTime, 
	MeetingCanceled,
	YouCanJoinUs,
	OkIWillJoinYou
};


inline std::string MsgToStr(int msg)
{
	switch (msg)
	{
	case Yes:

		return "Yes";

	case No:

		return "No";

	case CanWeMeet:

		return "Can we meet?";

	case ImBusy:

		return "I'm Busy";

	case YesWhatTimeWeMeet:

		return "Yes what time we meet?";

	case MeetingTime:

		return "Meeting time";

	case OkIWillComeAtThisTime:

		return "Ok I will come";

	case MeetingCanceled:

		return "Meeting was canceled";

	case YouCanJoinUs:

		return "You can join us";

	case OkIWillJoinYou:

		return "Ok I will join you";

	default:

		return "Not recognized!";
	}
}
