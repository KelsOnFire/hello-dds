#ifdef ACE_AS_STATIC_LIBS
#  include <dds/DCPS/RTPS/RtpsDiscovery.h>
#  include <dds/DCPS/transport/rtps_udp/RtpsUdp.h>
#endif

#include "LazyDDS.h"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[]){
  try {

    LazyDDS *myLazyDDS = new LazyDDS(argc, argv);

    myLazyDDS->setPublisher();

    // Write samples
    ACE_Time_Value quarterSecond( 0, 1000000 ); // 1s
    while (true) {
      myLazyDDS->waitSamplesToBeAcknowledged({30, 0});

      myLazyDDS->postHello({country: "FR", text: "Salut !"});
      ACE_OS::sleep( quarterSecond );

      myLazyDDS->postHello({country: "US", text: "Hello!"});
      // myLazyDDS->postHello({country: "ES", text: "Hola !"});

      ACE_OS::sleep( quarterSecond );
    }

    // Clean-up!
    myLazyDDS->cleanUp();

  } catch (const CORBA::Exception& e) {
    e._tao_print_exception("Exception caught in main():");
    return 1;
  }

  return 0;
}