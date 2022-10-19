#include "markerwidget.h"
#include "markerinfowidget.h"

MarkerWidget::MarkerWidget (const Event &event)
    : Wt::WPushButton (event.title), event_ (event)
{
  this->clicked ().connect (this, &MarkerWidget::info);
  this->setStyle ();
}

void
MarkerWidget::info ()
{
  info_ = std::make_unique<MarkerInfoWidget> (this->event_);
}

void
MarkerWidget::setStyle ()
{
}

std::unique_ptr<MarkerInfoWidget>
MarkerWidget::infoWidget ()
{
  return std::move (this->info_);
}
