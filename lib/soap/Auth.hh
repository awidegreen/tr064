#ifndef SOAPAUTH_HH
#define SOAPAUTH_HH

// pugi
#include <pugixml/pugixml.hpp>
// stl
#include <string>

namespace tr064
{
namespace soap
{

class Auth
{
public:
  enum State { 
    IDLE, 
    INIT_CHALLANGE, 
    INIT_CHALLANGE_RESPONSE_HANDLED, 
    AUTHENTICATED };

  Auth(const std::string& userid, const std::string& password) :
    _state(IDLE),
    _user(userid),
    _password(password) 
  { }

  void handle_auth_response(const pugi::xml_node& envelope_node);
  std::string get_auth_header_for_request();

  bool is_authenticated() const { return _state == AUTHENTICATED; }

  State get_current_state() const { return _state; }

private:
  static std::string state2str(State state);

  void update_nonce_realm(const pugi::xml_node& node);


  State _state;
  std::string _user;
  std::string _password;
  std::string _nonce;
  std::string _realm;
};


} // ns
} // ns

#endif /* SOAPAUTH_HH */
