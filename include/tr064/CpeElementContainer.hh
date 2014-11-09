#ifndef CPEELEMENTCONTAINER_HH
#define CPEELEMENTCONTAINER_HH

#include "Device.hh"
#include "Service.hh"
#include "ServiceAction.hh"

namespace tr064
{

class CpeElementContainer
{
public:
  virtual ~CpeElementContainer() { }

  virtual void add_device(const Device::Ptr& device) = 0;
  virtual void add_service(const Service::Ptr& service) = 0;
  virtual void add_action(const ServiceAction::Ptr& action) = 0;
};

}

#endif /* CPEELEMENTCONTAINER_HH */
