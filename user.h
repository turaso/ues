#ifndef USER_H
#define USER_H

#include "event.h"

#include <Wt/Auth/Dbo/AuthInfo.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/Types.h>
#include <Wt/WGlobal.h>

namespace dbo = Wt::Dbo;

class User;
using AuthInfo = Wt::Auth::Dbo::AuthInfo<User>;

class Event;

class User
{
public:
  std::string username;
  std::string password;
  dbo::collection<dbo::ptr<Event> > events;

  template <class Action>
  void
  persist (Action &a)
  {
    dbo::field (a, username, "username");
    dbo::field (a, password, "password");
    dbo::hasMany (a, events, dbo::ManyToMany, "participant_event");
  }
};

#endif // USER_H
