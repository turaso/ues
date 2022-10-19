#ifndef MARKERWIDGET_H
#define MARKERWIDGET_H

#include <Wt/WPushButton.h>
#include <Wt/WSignal.h>
#include <string>

#include "event.h"
#include "markerinfowidget.h"

class MarkerWidget : public Wt::WPushButton
{
public:
  Wt::Signals::Signal<const std::shared_ptr<Event> > showInfo;

  /*!
   * \brief Constructs marker interactable view
   * \param event is event based on which the interactable view is created
   */
  MarkerWidget (const Event &event);
  /*!
   * \brief A info widget getter
   * \return Ptr to a info widget
   */
  std::unique_ptr<MarkerInfoWidget> infoWidget ();

private:
  std::unique_ptr<MarkerInfoWidget> info_;

  void info ();
  const Event &event_;
  void setStyle ();
};

#endif // MARKERWIDGET_H
