/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef SVX_FORMDISPATCHINTERCEPTOR_HXX
#define SVX_FORMDISPATCHINTERCEPTOR_HXX

/** === begin UNO includes === **/
#include <com/sun/star/frame/XDispatchProviderInterceptor.hpp>
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
/** === end UNO includes === **/

#include <cppuhelper/compbase2.hxx>
#include <comphelper/uno3.hxx>

//........................................................................
namespace svxform
{
//........................................................................

	//====================================================================
	//= DispatchInterceptor
	//====================================================================
    class DispatchInterceptor
    {
    public:
        DispatchInterceptor() { }

        virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch> interceptedQueryDispatch(
            const ::com::sun::star::util::URL& aURL, const ::rtl::OUString& aTargetFrameName, sal_Int32 nSearchFlags) throw( ::com::sun::star::uno::RuntimeException ) = 0;

        virtual ::osl::Mutex* getInterceptorMutex() = 0;
    };

	//====================================================================
	//= 
	//====================================================================
    typedef ::cppu::WeakComponentImplHelper2<	::com::sun::star::frame::XDispatchProviderInterceptor
                                            ,	::com::sun::star::lang::XEventListener
                                            >	DispatchInterceptionMultiplexer_BASE;

    class DispatchInterceptionMultiplexer : public DispatchInterceptionMultiplexer_BASE
    {
        ::osl::Mutex	m_aFallback;
        ::osl::Mutex*   m_pMutex;

        // the component which's dispatches we're intercepting
        ::com::sun::star::uno::WeakReference< ::com::sun::star::frame::XDispatchProviderInterception >
                        m_xIntercepted;
        sal_Bool		m_bListening;

        // the real interceptor
        DispatchInterceptor*			m_pMaster;

        // chaining
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider>			m_xSlaveDispatcher;
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider>			m_xMasterDispatcher;

        virtual ~DispatchInterceptionMultiplexer();

    public:
        ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterception> getIntercepted() const { return m_xIntercepted; }

    public:
        DispatchInterceptionMultiplexer(
            const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProviderInterception>& _rToIntercept,
            DispatchInterceptor* _pMaster
        );

        // UNO
        DECLARE_UNO3_DEFAULTS(DispatchInterceptionMultiplexer, DispatchInterceptionMultiplexer_BASE);

        // ::com::sun::star::frame::XDispatchProvider
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > SAL_CALL queryDispatch( const ::com::sun::star::util::URL& aURL, const ::rtl::OUString& aTargetFrameName, sal_Int32 nSearchFlags ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > > SAL_CALL queryDispatches( const ::com::sun::star::uno::Sequence< ::com::sun::star::frame::DispatchDescriptor >& aDescripts ) throw(::com::sun::star::uno::RuntimeException);

        // ::com::sun::star::frame::XDispatchProviderInterceptor
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider > SAL_CALL getSlaveDispatchProvider(  ) throw(::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setSlaveDispatchProvider( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider >& xNewDispatchProvider ) throw(::com::sun::star::uno::RuntimeException);
        virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider > SAL_CALL getMasterDispatchProvider(	) throw(::com::sun::star::uno::RuntimeException);
        virtual void SAL_CALL setMasterDispatchProvider( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatchProvider >& xNewSupplier ) throw(::com::sun::star::uno::RuntimeException);

        // ::com::sun::star::lang::XEventListener
        virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw(::com::sun::star::uno::RuntimeException);

        // OComponentHelper
        virtual void SAL_CALL disposing();

    protected:
        void ImplDetach();
    };

//........................................................................
} // namespace svxform
//........................................................................

#endif // SVX_FORMDISPATCHINTERCEPTOR_HXX
