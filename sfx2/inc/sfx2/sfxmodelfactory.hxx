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



#ifndef SFX2_SFXMODELFACTORY_HXX
#define SFX2_SFXMODELFACTORY_HXX

#include "sfx2/dllapi.h"

/** === begin UNO includes === **/
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
/** === end UNO includes === **/

//........................................................................
namespace sfx2
{
//........................................................................

    #define SFXMODEL_STANDARD                   (sal_uInt64)(0x0000)
    #define SFXMODEL_EMBEDDED_OBJECT            (sal_uInt64)(0x0001)
    #define SFXMODEL_DISABLE_EMBEDDED_SCRIPTS   (sal_uInt64)(0x0002)
    #define SFXMODEL_DISABLE_DOCUMENT_RECOVERY  (sal_uInt64)(0x0004)

    typedef ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > ( SAL_CALL * SfxModelFactoryFunc ) (
        const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxFactory,
        const sal_uInt64 _nCreationFlags
    );

	//====================================================================
	//= createSfxModelFactory
	//====================================================================
    /** creates a XSingleServiceFactory which can be used to created instances
        of classes derived from SfxBaseModel

        In opposite to the default implementations from module cppuhelper, this
        factory evaluates certain creation arguments (passed to createInstanceWithArguments)
        and passes them to the factory function of the derived class.
    */
    ::com::sun::star::uno::Reference< ::com::sun::star::lang::XSingleServiceFactory >
        SFX2_DLLPUBLIC createSfxModelFactory(
            const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >& _rxServiceFactory,
            const ::rtl::OUString& _rImplementationName,
            const SfxModelFactoryFunc _pComponentFactoryFunc,
            const ::com::sun::star::uno::Sequence< ::rtl::OUString >& _rServiceNames
        );

//........................................................................
} // namespace sfx2
//........................................................................

#endif // SFX2_SFXMODELFACTORY_HXX
