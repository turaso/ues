#ifndef SESSION_H
#define SESSION_H

#include "event.h"
#include "eventdatabase.h"
#include "user.h"

#include <Wt/Auth/AbstractUserDatabase.h>
#include <Wt/Auth/Dbo/UserDatabase.h>
#include <Wt/Auth/Login.h>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/Session.h>
#include <Wt/Dbo/ptr.h>

namespace dbo = Wt::Dbo;

using UserDatabase = Wt::Auth::Dbo::UserDatabase<AuthInfo>;

class Session : public dbo::Session
{
public:
  static void configureAuth ();

  Session (const std::string &db);

  dbo::ptr<User> user () const;

  Wt::Auth::AbstractUserDatabase &users ();
  Wt::Auth::Login &
  login ()
  {
    return login_;
  }

  static const Wt::Auth::AuthService &auth ();
  static const Wt::Auth::PasswordService &passwordService ();
  static const std::vector<const Wt::Auth::OAuthService *> oAuth ();

private:
  std::unique_ptr<UserDatabase> users_;
  //  std::unique_ptr<MarkerDatabase> markers_;
  Wt::Auth::Login login_;
};

#endif // SESSION_H
