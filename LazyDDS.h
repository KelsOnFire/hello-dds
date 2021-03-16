#ifndef DEF_LAZYDDS
#define DEF_LAZYDDS

#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsSubscriptionC.h>

#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/WaitSet.h>

#include <dds/DCPS/StaticIncludes.h>

#include "HelloTypeSupportImpl.h"
#include <iostream>
#include <string>


class LazyDDS {
    private:
    DDS::DomainParticipantFactory_var dpf;
    DDS::DomainParticipant_var participant;
    DDS::Topic_var topic;
    DDS::Subscriber_var subscriber;
    DDS::Publisher_var publisher;
    DDS::DataReaderListener_var listener;
    DDS::WaitSet_var ws;
    DDS::StatusCondition_var condition;
    DDS::ConditionSeq conditions;
    DDS::SubscriptionMatchedStatus sub_matches;
    DDS::PublicationMatchedStatus pub_matches;
    DDS::Duration_t timeout;
    CORBA::String_var type_name;

    DDS::DataWriter_var writer;
    HelloModule::HelloTypeSupport_var ts;
    HelloModule::HelloDataWriter_var hello_writer;

    DDS::Publisher_var createPublisher();
    DDS::DataWriter_var createWriter(DDS::Topic_var p_topic);
    DDS::Topic_var createHelloTopic(const char * p_topic_name, HelloModule::HelloTypeSupport_var p_ts);
    void subscribeTopic();
    DDS::DomainParticipant_var createParticipant(int p_domain_id);

    public:
    LazyDDS(int &argc, ACE_TCHAR *argv[]);
    void setPublisher();
    void log(std::string message);
    void logError(std::string message);
    void postMessage();
    void getMessage();
    void postHello(HelloModule::Hello hello);
    void waitSamplesToBeAcknowledged(DDS::Duration_t p_timeout);
    void cleanUp();
};

#endif