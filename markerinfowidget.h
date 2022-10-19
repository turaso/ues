#ifndef MARKERINFOWIDGET_H
#define MARKERINFOWIDGET_H

#include <Wt/WContainerWidget.h>
#include <Wt/WSignal.h>

#include "event.h"

class MarkerInfoWidget
{
public:
  Wt::Signal<const std::shared_ptr<Event> > showEventInfo;
  Wt::Signal<> hideEventInfo;
  Wt::Signal<const std::shared_ptr<Event> > showParticipants;

  /*!
   * \brief Constructs widget creator but not widget
   * \param event is event based on which widget is created
   */
  MarkerInfoWidget (const Event &event);
  /*!
   * \brief Creates event info view
   * \return Ptr to the view
   */
  std::unique_ptr<Wt::WContainerWidget> createWidget () const;

private:
  std::shared_ptr<Event> event_;
};

#endif // MARKERINFOWIDGET_H
