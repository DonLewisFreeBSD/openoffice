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


#ifndef _SVX_TBXCUSTOMSHAPES_HXX
#define _SVX_TBXCUSTOMSHAPES_HXX

#include <sfx2/tbxctrl.hxx>
#include "svx/svxdllapi.h"

/*************************************************************************
|*
|* SvxTbxCtlCustomShapes
|*
\************************************************************************/

class SVX_DLLPUBLIC SvxTbxCtlCustomShapes : public SfxToolBoxControl
{
public:
    using SfxToolBoxControl::Select;
	virtual void				Select( sal_Bool bMod1 = sal_False );
	virtual void				StateChanged( sal_uInt16 nSID, SfxItemState eState,
											  const SfxPoolItem* pState );
	virtual SfxPopupWindowType	GetPopupWindowType() const;
	virtual SfxPopupWindow*		CreatePopupWindow();

			SFX_DECL_TOOLBOX_CONTROL();

			SvxTbxCtlCustomShapes( sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx );
			~SvxTbxCtlCustomShapes() {}

	//interface XSubToolbarController:
    virtual ::sal_Bool SAL_CALL opensSubToolbar() throw (::com::sun::star::uno::RuntimeException);
    virtual ::rtl::OUString SAL_CALL getSubToolbarName() throw (::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL functionSelected( const ::rtl::OUString& aCommand ) throw (::com::sun::star::uno::RuntimeException);
    virtual void SAL_CALL updateImage() throw (::com::sun::star::uno::RuntimeException);

private:
    rtl::OUString		m_aSubTbName;
    rtl::OUString		m_aSubTbxResName;
    rtl::OUString		m_aCommand;
};


#endif

