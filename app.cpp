#include "app.h"

#include <Wt/Auth/AuthWidget.h>
#include <Wt/Auth/PasswordService.h>
#include <Wt/WBootstrapTheme.h>
#include <Wt/WHBoxLayout.h>
#include <Wt/WLeafletMap.h>
#include <Wt/WServer.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>

#include <functional>

#include "markerformwidget.h"
#include "markerinfowidget.h"
#include "markerwidget.h"

App::App (const Wt::WEnvironment &env)
    : Wt::WApplication (env), session_ ("unienet"),
      eventDb_ (session_) // ? check_me
{
  this->setTitle ("unienet");

  this->root ()->addStyleClass ("container");
  this->setTheme (std::make_shared<Wt::WBootstrapTheme> ());
  this->useStyleSheet ("style.css");

  session_.login ().changed ().connect (this, &App::authEvent);

  std::unique_ptr<Wt::Auth::AuthWidget> authWidget
      = std::make_unique<Wt::Auth::AuthWidget> (
          Session::auth (), session_.users (), session_.login ());

  authWidget->model ()->addPasswordAuth (&Session::passwordService ());
  authWidget->model ()->addOAuth (Session::oAuth ());
  authWidget->setRegistrationEnabled (true);

  authWidget->processEnvironment ();

  this->root ()->addWidget (std::move (authWidget));
}

void
App::authEvent ()
{
  if (session_.login ().loggedIn ())
    {
      const Wt::Auth::User &u = session_.login ().user ();
      log ("notice") << "User " << u.id () << " ("
                     << u.identity (Wt::Auth::Identity::LoginName) << ")"
                     << " logged in.";
      this->createMainView ();
    }
  else
    {
      log ("notice") << "User logged out.";
    }
}

void
App::createMainView ()
{
  auto layout = std::make_unique<Wt::WHBoxLayout> ();
  auto map = std::make_unique<Wt::WLeafletMap> ();
  Wt::Json::Object options;
  options["maxZoom"] = 20;
  options["attribution"] = "&copy; <a "
                           "href=\"https://www.openstreetmap.org/"
                           "copyright\">OpenStreetMap</a> contributors";
  map->addTileLayer ("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png",
                     options);
  const std::pair<double, double> coords = { 55.7930907, 49.1143051 };
  map->panTo ({ coords.first, coords.second });
  //  Borders mapBorders = this->getMapBorders (map, coords);
  //  std::vector<Event> events
  //      = eventDb_.getEvents ({ mapBorders.up, mapBorders.left },
  //                            { mapBorders.down, mapBorders.right });
  this->updateEvents (map.get ());
  map->zoomLevelChanged ().connect (
      [&map, this] (int zoom) { this->updateEvents (map.get ()); });
  layout->addWidget (std::move (map));
  this->root ()->setLayout (std::move (layout));
  this->root ()->mouseDragged ().connect (
      [&map, this] (Wt::WMouseEvent mE) { this->updateEvents (map.get ()); });
}

std::unique_ptr<Wt::WApplication>
createApplication (const Wt::WEnvironment &env)
{
  return std::make_unique<App> (env);
}

// Borders
// App::getMapBorders (const std::unique_ptr<Wt::WLeafletMap> &map,
//                    const std::pair<double, double> coords) const
//{
//  Borders borders;
//  borders.up = coords.second +
//}

void
App::updateEvents (Wt::WLeafletMap *map)
{
  constexpr int coef = 8;
  constexpr int width = 1920 * coef;
  constexpr int height = 1080 * coef;
  constexpr double maxLatitude = 90;
  constexpr double maxLongitude = 180;
  constexpr int zeroZoomMapSize = 256;
  const Wt::WLeafletMap::Coordinate position = map->position ();
  const int zoom
      = map->zoomLevel (); // world size in pixels = {256, 256} * 2^zoom
  const int mapSizeInPx = zeroZoomMapSize * 1 << zoom;
  std::pair<double, double> upLeft;
  std::pair<double, double> downRight;
  if (mapSizeInPx >= width || mapSizeInPx >= height)
    {
      upLeft = { maxLatitude * height / mapSizeInPx + position.latitude (),
                 -maxLongitude * width / mapSizeInPx + position.longitude () };
      downRight = upLeft
          = { -maxLatitude * height / mapSizeInPx + position.latitude (),
              maxLongitude * width / mapSizeInPx + position.longitude () };
    }
  else
    {
      upLeft = { maxLatitude, -maxLongitude };
      downRight = { -maxLatitude, maxLongitude };
    }

  std::vector<Event> events = this->eventDb_.getEvents (upLeft, downRight);
  for (Event &event : events)
    {
      const Wt::WLeafletMap::Coordinate coords = { event.x, event.y };
      std::unique_ptr<MarkerWidget> mWidget
          = std::make_unique<MarkerWidget> (event);
      std::unique_ptr<Wt::WLeafletMap::WidgetMarker> marker
          = std::make_unique<Wt::WLeafletMap::WidgetMarker> (
              coords, std::move (mWidget));
      ((MarkerWidget *)marker->widget ())
          ->showInfo.connect (
              [this, &marker] (const std::shared_ptr<Event> event) {
                this->showEventInfo (std::shared_ptr<MarkerWidget> (
                    (MarkerWidget *)marker->widget ()));
              });
      map->addMarker (std::move (marker));
    }
}

void
App::showEventInfo (const std::shared_ptr<MarkerWidget> markerWidget)
{
  this->cleanEventPlace ();
  this->eventWidget_ = markerWidget->infoWidget ()->createWidget ().get ();
  this->root ()->layout ()->addWidget (
      std::unique_ptr<Wt::WContainerWidget> (this->eventWidget_));
  markerWidget->infoWidget ()->hideEventInfo.connect (this,
                                                      &App::hideEventInfo);
  markerWidget->infoWidget ()->showParticipants.connect (
      this, &App::showEventParticipants);
}

void
App::hideEventInfo ()
{
  this->root ()->layout ()->removeWidget (this->eventWidget_);
}

void
App::showEventParticipants ()
{
  //  this->root ()->layout ()->addWidget ()
}

void
App::hideEventParticipants ()
{
  //  this->root ()->layout ()->removeWidget ();
}

void
App::showEventReg ()
{
  this->cleanEventPlace ();
  this->eventWidget_ = std::make_unique<MarkerFormWidget> ().get ();
  this->root ()->layout ()->addWidget (
      std::unique_ptr<Wt::WContainerWidget> (this->eventWidget_)); // ?! check
}

void
App::hideEventReg ()
{
  this->root ()->layout ()->removeWidget (this->eventWidget_);
}

void
App::cleanEventPlace ()
{
  if (this->eventWidget_ == nullptr)
    {
      this->root ()->layout ()->removeWidget (this->eventWidget_);
      this->eventWidget_ = nullptr;
    }
}

int
main (int argc, char **argv)
{
  try
    {
      Wt::WServer server{ argc, argv, WTHTTP_CONFIGURATION };

      server.addEntryPoint (Wt::EntryPointType::Application,
                            createApplication);

      Session::configureAuth ();

      server.run ();
    }
  catch (Wt::WServer::Exception &e)
    {
      std::cerr << e.what () << std::endl;
    }
  catch (Wt::Dbo::Exception &e)
    {
      std::cerr << "Dbo exception: " << e.what () << std::endl;
    }
  catch (std::exception &e)
    {
      std::cerr << "exception: " << e.what () << std::endl;
    }
}

// int
// main (int argc, char **argv)
//{
//  return Wt::WRun (argc, argv, [] (const
//  Wt::WEnvironment &env) {
//    return std::make_unique<App> (env);
//  });
//}
