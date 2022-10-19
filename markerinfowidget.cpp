#include "markerinfowidget.h"

#include <Wt/WHBoxLayout.h>
#include <Wt/WLabel.h>
#include <Wt/WPushButton.h>
#include <Wt/WString.h>
#include <Wt/WText.h>
#include <Wt/WVBoxLayout.h>

MarkerInfoWidget::MarkerInfoWidget (const Event &event)
{
  this->event_ = std::make_shared<Event> (event);
  showEventInfo.emit (event_);
}

std::unique_ptr<Wt::WContainerWidget>
MarkerInfoWidget::createWidget () const
{
  std::unique_ptr<Wt::WContainerWidget> widget
      = std::make_unique<Wt::WContainerWidget> ();

  std::unique_ptr<Wt::WVBoxLayout> vLayout
      = std::make_unique<Wt::WVBoxLayout> ();

  std::unique_ptr<Wt::WHBoxLayout> topHLayout
      = std::make_unique<Wt::WHBoxLayout> ();
  topHLayout->addWidget (std::make_unique<Wt::WText> (
                             Wt::WString ("{1}").arg (this->event_->title)),
                         1);
  auto participantsButton = std::make_unique<Wt::WPushButton> (
      std::to_string (this->event_->participants.size ()) + "/"
      + this->event_
            ->participantsLimit_to_string ()); // ? to_string() template
  participantsButton->clicked ().connect (
      [this] () { this->showParticipants.emit (this->event_); });
  topHLayout->addWidget (std::move (participantsButton));
  topHLayout->addWidget (std::make_unique<Wt::WLabel> (), 1);
  auto closeButton = std::make_unique<Wt::WPushButton> ("close");
  closeButton->clicked ().connect ([this] () { this->hideEventInfo.emit (); });
  topHLayout->addWidget (std::move (closeButton));
  vLayout->addLayout (std::move (topHLayout));

  std::unique_ptr<Wt::WHBoxLayout> dateHLayout
      = std::make_unique<Wt::WHBoxLayout> ();
  dateHLayout->addWidget (
      std::make_unique<Wt::WLabel> (this->event_->datetimeStart.toString ()));
  dateHLayout->addWidget (std::make_unique<Wt::WLabel> (" - "));
  dateHLayout->addWidget (std::make_unique<Wt::WLabel> (
      this->event_->datetimeFinish_to_string ())); // ? to_string() template
  vLayout->addLayout (std::move (dateHLayout));

  vLayout->addWidget (std::make_unique<Wt::WText> (this->event_->description));

  widget->setLayout (std::move (vLayout));

  return widget;
}
