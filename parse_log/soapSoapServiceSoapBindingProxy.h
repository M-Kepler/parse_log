/* soapSoapServiceSoapBindingProxy.h
   Generated by gSOAP 2.8.70 for lbmriskwarning.h

gSOAP XML Web services tools
Copyright (C) 2000-2018, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#ifndef soapSoapServiceSoapBindingProxy_H
#define soapSoapServiceSoapBindingProxy_H
#include "soapH.h"

    class SOAP_CMAC SoapServiceSoapBindingProxy {
      public:
        /// Context to manage proxy IO and data
        struct soap *soap;
        bool soap_own; ///< flag indicating that this context is owned by this proxy when context is shared
        /// Endpoint URL of service 'SoapServiceSoapBindingProxy' (change as needed)
        const char *soap_endpoint;
        /// Variables globally declared in lbmriskwarning.h, if any
        /// Construct a proxy with new managing context
        SoapServiceSoapBindingProxy();
        /// Copy constructor
        SoapServiceSoapBindingProxy(const SoapServiceSoapBindingProxy& rhs);
        /// Construct proxy given a shared managing context
        SoapServiceSoapBindingProxy(struct soap*);
        /// Construct proxy given a shared managing context and endpoint URL
        SoapServiceSoapBindingProxy(struct soap*, const char *endpoint);
        /// Constructor taking an endpoint URL
        SoapServiceSoapBindingProxy(const char *endpoint);
        /// Constructor taking input and output mode flags for the new managing context
        SoapServiceSoapBindingProxy(soap_mode iomode);
        /// Constructor taking endpoint URL and input and output mode flags for the new managing context
        SoapServiceSoapBindingProxy(const char *endpoint, soap_mode iomode);
        /// Constructor taking input and output mode flags for the new managing context
        SoapServiceSoapBindingProxy(soap_mode imode, soap_mode omode);
        /// Destructor deletes non-shared managing context only (use destroy() to delete deserialized data)
        virtual ~SoapServiceSoapBindingProxy();
        /// Initializer used by constructors
        virtual void SoapServiceSoapBindingProxy_init(soap_mode imode, soap_mode omode);
        /// Return a copy that has a new managing context with the same engine state
        virtual SoapServiceSoapBindingProxy *copy();
        /// Copy assignment
        SoapServiceSoapBindingProxy& operator=(const SoapServiceSoapBindingProxy&);
        /// Delete all deserialized data (uses soap_destroy() and soap_end())
        virtual void destroy();
        /// Delete all deserialized data and reset to default
        virtual void reset();
        /// Disables and removes SOAP Header from message by setting soap->header = NULL
        virtual void soap_noheader();
        /// Get SOAP Header structure (i.e. soap->header, which is NULL when absent)
        virtual ::SOAP_ENV__Header *soap_header();
        /// Get SOAP Fault structure (i.e. soap->fault, which is NULL when absent)
        virtual ::SOAP_ENV__Fault *soap_fault();
        /// Get SOAP Fault string (NULL when absent)
        virtual const char *soap_fault_string();
        /// Get SOAP Fault detail as string (NULL when absent)
        virtual const char *soap_fault_detail();
        /// Close connection (normally automatic, except for send_X ops)
        virtual int soap_close_socket();
        /// Force close connection (can kill a thread blocked on IO)
        virtual int soap_force_close_socket();
        /// Print fault
        virtual void soap_print_fault(FILE*);
    #ifndef WITH_LEAN
    #ifndef WITH_COMPAT
        /// Print fault to stream
        virtual void soap_stream_fault(std::ostream&);
    #endif
        /// Write fault to buffer
        virtual char *soap_sprint_fault(char *buf, size_t len);
    #endif
        /// Web service operation 'doService' (returns SOAP_OK or error code)
        virtual int doService(ns1__doService *ns1__doService_, ns1__doServiceResponse &ns1__doServiceResponse_)
        { return this->doService(NULL, NULL, ns1__doService_, ns1__doServiceResponse_); }
        virtual int doService(const char *soap_endpoint, const char *soap_action, ns1__doService *ns1__doService_, ns1__doServiceResponse &ns1__doServiceResponse_);
    };
#endif