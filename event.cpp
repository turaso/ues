#include "event.h"

Wt::WString
Event::datetimeFinish_to_string ()
{
  if (this->datetimeFinish.isValid ())
    {
      return this->datetimeFinish.toString ();
    }
  else
    {
      return "Unknown";
    }
}

Wt::WString
Event::participantsLimit_to_string ()
{
  if (this->participantsLimit == 0)
    {
      return "∞";
    }
  else if (this->participantsLimit == 1)
    {
      return "?";
    }
  else
    {
      return Wt::WString::tr (std::to_string (this->participantsLimit));
    }
}
