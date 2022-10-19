#ifndef MARKERDATABASE_H
#define MARKERDATABASE_H

#include "event.h"

#include <Wt/Dbo/Session.h>
#include <vector>

class EventDatabase
{
private:
  Wt::Dbo::Session &s_;

public:
  EventDatabase (Wt::Dbo::Session &);
  /*!
   * \brief Saves event to the DB
   * \return if event was saved
   */
  bool addEvent (const Event &);
  /*!
   * \brief Finds events in the rectangle with known corners
   * \param upLeftBorder - (up latitude, left longitude) coordinate
   * \param downRightBorder - (down latitude, right longitude) coordinate
   * \return Found events
   */
  std::vector<Event>
  getEvents (const std::pair<double, double> upLeftBorder,
             const std::pair<double, double> downRightBorder) const;
};

#endif // MARKERDATABASE_H
