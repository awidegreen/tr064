#ifndef CPE_TEST_GLOBAL_HH
#define CPE_TEST_GLOBAL_HH

#include <tr064/CpeElementContainer.hh>
#include <tr064/ResourceRetrieval.hh>

// ggogle test
#include <gmock/gmock.h>

using namespace tr064;

static ResourceRetreival::Ptr RESOUCRE_RETRIEVAL_FILE(new ResourceRetreivalFile());
static ResourceRetreival::Ptr RESOUCRE_RETRIEVAL_EMPTY(new ResourceRetreivalEmpty());

class MockCpeElementContainer : 
  public CpeElementContainer
{
public:
  MOCK_METHOD1(add_service, void(const Service::Ptr&));
  MOCK_METHOD1(add_device, void(const Device::Ptr&));
  MOCK_METHOD1(add_action, void(const ServiceAction::Ptr&));
};

#endif /* CPE_TEST_GLOBAL_HH */
