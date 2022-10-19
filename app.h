#ifndef APP_H
#define APP_H

#include <Wt/WApplication.h>

#include "eventdatabase.h"
#include "markerwidget.h"
#include "session.h"

struct Borders;

class App : public Wt::WApplication
{
public:
  App (const Wt::WEnvironment &env);

private:
  Session session_;
  EventDatabase eventDb_;
  Wt::WContainerWidget *eventWidget_;

  void authEvent ();
  void createMainView ();
  void updateEvents (Wt::WLeafletMap *map);
  //  void
  //  updateEvents (int zoom, Wt::WLeafletMap *map)
  //  {
  //    this->updateEvents (map);
  //  }
  //  void
  //  updateEvents (Wt::WMouseEvent mouseEvent, Wt::WLeafletMap *map)
  //  {
  //    this->updateEvents (map);
  //  }
  Borders getMapBorders (const std::unique_ptr<Wt::WLeafletMap> &map,
                         const std::pair<double, double> coords) const;
  void showEventInfo (const std::shared_ptr<MarkerWidget> markerWidget);
  void hideEventInfo ();
  void showEventParticipants ();
  void hideEventParticipants ();
  void showEventReg ();
  void hideEventReg ();
  void cleanEventPlace ();
};

struct Borders
{
  double up;
  double left;
  double down;
  double right;
};

#endif // APP_H
