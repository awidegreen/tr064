#ifndef SOAPSESSION_HH
#define SOAPSESSION_HH

#include <tr064/Service.hh>
#include <tr064/ServiceAction.hh>

#include <pugixml/pugixml.hpp>

#include "Auth.hh"

// stl
#include <string>

namespace tr064
{
namespace soap
{

/**
 * User authendication is required by default!
 */
class Session
{
public:
  Session(
      const std::string& host,
      const std::string& port,
      const std::string& user,
      const std::string& password) :
    _host(host),
    _port(port),
    _auth(user, password),
    _no_auth(false)
  { }

  /**
   * Executes the given action. The return value it the xml structure associated
   * to the actions. e.g. 
   *
   * Action: GetInfo Service: DeviceInfo
   * 
   * <u:GetInfoResponse xmlns:u="urn:dslforum-org:service:DeviceInfo:1">
   *    <NewManufacturerName>AVM</NewManufacturerName>
   *    ... other out args ...
   * </u:GetInfoResponse>
   * @retrun The response xml node associated to the action.
   */
  pugi::xml_node
  execute_action(
      const Service::Ptr& service, 
      const ServiceAction::Ptr& action);

  void disable_auth() { _no_auth = true; }

private:
  std::string get_headers(
      const Service::Ptr& service,
      const ServiceAction::Ptr& action,
      const size_t body_length);

  std::string get_body(
      const Service::Ptr& service,
      const ServiceAction::Ptr& action);

  std::string _host;
  std::string _port;
  Auth _auth;
  bool _no_auth;
};

} // ns
} // ns

#endif /* SOAPSESSION_HH */
