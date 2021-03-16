/**
 * 
 */
#include "LazyDDS.h"

void LazyDDS::log(std::string p_message) {
    std::cout << p_message << std::endl;
}

void LazyDDS::logError(std::string p_message) {
    std::cerr << "ERROR: " + p_message << std::endl;
}

DDS::DomainParticipant_var LazyDDS::createParticipant(int p_domain_id) {
    // Create DomainParticipant
    DDS::DomainParticipant_var m_participant = dpf->create_participant(p_domain_id,
        PARTICIPANT_QOS_DEFAULT,
        0,
        OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    
    if (!m_participant) {
      logError("create_participant failed!");
    }

    return m_participant;
}

DDS::Topic_var LazyDDS::createHelloTopic(const char * p_topic_name, HelloModule::HelloTypeSupport_var p_ts) {
    // Register TypeSupport (HelloModule::Hello)
    if (p_ts->register_type(participant, "") != DDS::RETCODE_OK) {
      logError("register_type failed!");
    }

    // Create Topic (Hello World)
    DDS::Topic_var m_topic;
    type_name = p_ts->get_type_name(); 
    m_topic = participant->create_topic(p_topic_name,
        type_name,
        TOPIC_QOS_DEFAULT,
        0,
        OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!m_topic) {
      logError("create_topic failed!");
    }

    return m_topic;
}

DDS::Publisher_var LazyDDS::createPublisher() {
    DDS::Publisher_var m_publisher = participant->create_publisher(PUBLISHER_QOS_DEFAULT,
        0,
        OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!m_publisher) {
      logError("create_publisher failed!");
    }

    return m_publisher;
}

DDS::DataWriter_var LazyDDS::createWriter(DDS::Topic_var p_topic) {
    DDS::DataWriter_var m_writer = publisher->create_datawriter(p_topic,
        DATAWRITER_QOS_DEFAULT,
        0,
        OpenDDS::DCPS::DEFAULT_STATUS_MASK);

    if (!m_writer) {
      logError("create_datawriter failed!");
    }

    return m_writer;
}

LazyDDS::LazyDDS(int &argc, ACE_TCHAR *argv[]) {
    log("INFO: Start LazyDDS");

    // Initialize DomainParticipantFactory
    dpf = TheParticipantFactoryWithArgs(argc, argv);

    // Create DomainParticipant
    participant = createParticipant(42);

    topic = createHelloTopic("Hello World", new HelloModule::HelloTypeSupportImpl);
}

void LazyDDS::setPublisher() {
    // Create Publisher
    publisher = createPublisher();

    // Create DataWriter
    writer = createWriter(topic);

    hello_writer = HelloModule::HelloDataWriter::_narrow(writer);

    if (!hello_writer) {
      logError("_narrow failed!");
    }

    // Block until Subscriber is available
    condition = writer->get_statuscondition();
    condition->set_enabled_statuses(DDS::PUBLICATION_MATCHED_STATUS);

    ws = new DDS::WaitSet;
    ws->attach_condition(condition); // NOTE: If commented  => wait failed error 

    while (true) {
      if (writer->get_publication_matched_status(pub_matches) != ::DDS::RETCODE_OK) {
        logError("get_publication_matched_status failed!");
      }

      if (pub_matches.current_count >= 1) {
        break;
      }

      // DDS::ConditionSeq conditions;
      timeout = { 60, 0 };
      if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
        logError("wait failed!");
      }
    }
}

// Post {country, text} message to Topic "Hello World" 
void LazyDDS::postHello(HelloModule::Hello hello) {
  DDS::ReturnCode_t error = hello_writer->write(hello, DDS::HANDLE_NIL);
  
  log("Message envoyé");

  if (error != DDS::RETCODE_OK) {
      logError("postHello !");
  }
}

// Wait for samples to be acknowledged
void LazyDDS::waitSamplesToBeAcknowledged(DDS::Duration_t p_timeout) {
    if (hello_writer->wait_for_acknowledgments(p_timeout) != DDS::RETCODE_OK) {
      logError("wait_for_acknowledgments failed!");
    }
}

// Clean-up!
void LazyDDS::cleanUp() {
    try {
        if(!CORBA::is_nil(participant.in())) {
            participant->delete_contained_entities();
        }

        if(!CORBA::is_nil(dpf.in())) {
            dpf->delete_participant(participant);
        }
    } catch (CORBA::Exception& e) {
        logError("Exception caught in cleanUp");
        std::cerr << e << std::endl;
        ACE_OS::exit(1);
    }
    TheServiceParticipant->shutdown();
}