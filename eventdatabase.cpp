#include "eventdatabase.h"

EventDatabase::EventDatabase (Wt::Dbo::Session &s) : s_ (s) {}

bool
EventDatabase::addEvent (const Event &event)
{
  Wt::Dbo::Transaction transaction (this->s_);
  auto ptr = s_.add<Event> (std::make_unique<Event> (std::move (event)));
  return (bool)ptr; // ? '!'
}

std::vector<Event> // ? unique_ptr
EventDatabase::getEvents (
    const std::pair<double, double> upLeftBorder,
    const std::pair<double, double> downRightBorder) const
{
  std::vector<Event> events;
  Wt::Dbo::Transaction transaction (this->s_);
  Wt::Dbo::collection<Wt::Dbo::ptr<Event> > collection
      = s_.find<Event> ("where y < ? and x > ? and y > ? and x < ? and "
                        "datetimeStart > ?")
            .bind (upLeftBorder.first)
            .bind (upLeftBorder.second)
            .bind (downRightBorder.first)
            .bind (downRightBorder.second)
            .bind (Wt::WDateTime (std::chrono::system_clock::now ()));
  // ? transaction end
  for (Wt::Dbo::ptr<Event> event : collection)
    {
      events.push_back (*event.get ());
    }
  return events;
}
