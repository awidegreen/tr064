#ifndef CPEQUERY_HH
#define CPEQUERY_HH

#include "Service.hh"
#include "ServiceAction.hh"

// stl
#include <string>

namespace tr064
{


class CpeQuery
{
public:
  struct Credentials
  {
    std::string username;
    std::string password;
    std::string host;
    std::string port;
    std::string http_username;
    std::string http_password;
    bool empty() const { return username.empty() && password.empty(); }
  };

  /** ctor */
  CpeQuery(const Service::Ptr& service, const Credentials& credentials) : 
    _service(service),
    _credentials(credentials)
  {
  }

  void execute(const ServiceAction::Ptr& action) const;

private:
  Service::Ptr _service;
  Credentials _credentials;
};

} // namespace 

#endif /* CPEQUERY_HH */
