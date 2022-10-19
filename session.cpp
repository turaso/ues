#include "session.h"

#include <Wt/Auth/AuthService.h>
#include <Wt/Auth/Dbo/AuthInfo.h>
#include <Wt/Auth/GoogleService.h>
#include <Wt/Auth/HashFunction.h>
#include <Wt/Auth/OAuthService.h>
#include <Wt/Auth/PasswordService.h>
#include <Wt/Auth/PasswordStrengthValidator.h>
#include <Wt/Auth/PasswordVerifier.h>
#include <Wt/Dbo/backend/MySQL.h>

using namespace Wt;

namespace
{
Auth::AuthService myAuthService;
Auth::PasswordService myPasswordService (myAuthService);
std::vector<std::unique_ptr<Auth::OAuthService> > myOAuthServices;
}

void
Session::configureAuth ()
{
  myAuthService.setAuthTokensEnabled (true, "logincookie");
  myAuthService.setEmailVerificationEnabled (false);
  myAuthService.setEmailVerificationRequired (false);

  std::unique_ptr<Wt::Auth::PasswordVerifier> verifier
      = std::make_unique<Wt::Auth::PasswordVerifier> ();
  verifier->addHashFunction (
      std::make_unique<Wt::Auth::BCryptHashFunction> (7));
  myPasswordService.setVerifier (std::move (verifier));
  myPasswordService.setAttemptThrottlingEnabled (true);
  auto validator = std::make_unique<Wt::Auth::PasswordStrengthValidator> ();
  validator->setMinimumLength (Auth::PasswordStrengthType::OneCharClass, 4);
  validator->setMinimumLength (Auth::PasswordStrengthType::TwoCharClass, 4);
  validator->setMinimumLength (Auth::PasswordStrengthType::ThreeCharClass, 4);
  validator->setMinimumLength (Auth::PasswordStrengthType::FourCharClass, 4);
  validator->setMinimumLength (Auth::PasswordStrengthType::PassPhrase, 4);
  validator->setMinimumMatchLength (8);
  //  validator->setMinimumPassPhraseWords(1);
  myPasswordService.setStrengthValidator (std::move (validator));

  if (Wt::Auth::GoogleService::configured ())
    myOAuthServices.push_back (
        std::make_unique<Wt::Auth::GoogleService> (myAuthService));

  for (unsigned i = 0; i < myOAuthServices.size (); ++i)
    myOAuthServices[i]->generateRedirectEndpoint ();
}

Session::Session (const std::string &db)
{
  auto connection = std::make_unique<Dbo::backend::MySQL> (db, "root", "toor",
                                                           "localhost", 3306);

  connection->setProperty ("show-queries", "true");

  this->setConnection (std::move (connection));

  this->mapClass<User> ("user");
  this->mapClass<AuthInfo> ("auth_info");
  this->mapClass<AuthInfo::AuthIdentityType> ("auth_identity");
  this->mapClass<AuthInfo::AuthTokenType> ("auth_token");
  this->mapClass<Event> ("event");

  try
    {
      this->createTables ();
      std::cerr << "created tables" << std::endl;
    }
  catch (std::exception &e)
    {
      std::cerr << e.what () << std::endl;
      std::cerr << "using existing tables" << std::endl;
    }

  this->users_ = std::make_unique<UserDatabase> (*this);
}

Auth::AbstractUserDatabase &
Session::users ()
{
  return *this->users_;
}

dbo::ptr<User>
Session::user () const
{
  if (this->login_.loggedIn ())
    {
      dbo::ptr<AuthInfo> authInfo = this->users_->find (this->login_.user ());
      return authInfo->user ();
    }
  else
    {
      return dbo::ptr<User> ();
    }
}

const Auth::AuthService &
Session::auth ()
{
  return myAuthService;
}

const Auth::PasswordService &
Session::passwordService ()
{
  return myPasswordService;
}

const std::vector<const Auth::OAuthService *>
Session::oAuth ()
{
  std::vector<const Auth::OAuthService *> result;
  for (auto &auth : myOAuthServices)
    {
      result.push_back (auth.get ());
    }
  return result;
}
