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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svtools.hxx"

#include <svtools/fontsubstconfig.hxx>
#include <svl/svarray.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Sequence.hxx>
#include <tools/debug.hxx>

#include <vcl/outdev.hxx>
#include <rtl/logfile.hxx>

using namespace utl;
using namespace rtl;
using namespace com::sun::star;
using namespace com::sun::star::uno;
using namespace com::sun::star::beans;

#define C2U(cChar) OUString::createFromAscii(cChar)

const sal_Char cReplacement[] = "Replacement";
const sal_Char cFontPairs[] = "FontPairs";

const sal_Char cReplaceFont[] 	= "ReplaceFont";
const sal_Char cSubstituteFont[]= "SubstituteFont";
const sal_Char cOnScreenOnly[] 	= "OnScreenOnly";
const sal_Char cAlways[] 		= "Always";

//-----------------------------------------------------------------------------
typedef SubstitutionStruct* SubstitutionStructPtr;
SV_DECL_PTRARR_DEL(SubstitutionStructArr, SubstitutionStructPtr, 2, 2)
SV_IMPL_PTRARR(SubstitutionStructArr, SubstitutionStructPtr);
//-----------------------------------------------------------------------------
struct SvtFontSubstConfig_Impl
{
	SubstitutionStructArr	aSubstArr;
};
/* -----------------------------18.01.01 12:04--------------------------------

 ---------------------------------------------------------------------------*/
SvtFontSubstConfig::SvtFontSubstConfig() :
	ConfigItem(C2U("Office.Common/Font/Substitution")),
	bIsEnabled(sal_False),
	pImpl(new SvtFontSubstConfig_Impl)
{
	RTL_LOGFILE_CONTEXT(aLog, "svtools SvtFontSubstConfig::SvtFontSubstConfig()");
	
	Sequence<OUString> aNames(1);
	aNames.getArray()[0] = C2U(cReplacement);
	Sequence<Any> aValues = GetProperties(aNames);
	DBG_ASSERT(aValues.getConstArray()[0].hasValue(), "no value available");
	if(aValues.getConstArray()[0].hasValue())
		bIsEnabled = *(sal_Bool*)aValues.getConstArray()[0].getValue();

	OUString sPropPrefix(C2U(cFontPairs));
    Sequence<OUString> aNodeNames = GetNodeNames(sPropPrefix, CONFIG_NAME_LOCAL_PATH);
	const OUString* pNodeNames = aNodeNames.getConstArray();
	Sequence<OUString> aPropNames(aNodeNames.getLength() * 4);
	OUString* pNames = aPropNames.getArray();
	sal_Int32 nName = 0;
	sPropPrefix += C2U("/");
	sal_Int32 nNode;
	for(nNode = 0; nNode < aNodeNames.getLength(); nNode++)
	{
		OUString sStart(sPropPrefix);
		sStart += pNodeNames[nNode];
		sStart += C2U("/");
		pNames[nName] = sStart; 	pNames[nName++] += C2U(cReplaceFont);
		pNames[nName] = sStart; 	pNames[nName++] += C2U(cSubstituteFont);
		pNames[nName] = sStart; 	pNames[nName++] += C2U(cAlways);
		pNames[nName] = sStart; 	pNames[nName++] += C2U(cOnScreenOnly);
	}
	Sequence<Any> aNodeValues = GetProperties(aPropNames);
	const Any* pNodeValues = aNodeValues.getConstArray();
	nName = 0;
	for(nNode = 0; nNode < aNodeNames.getLength(); nNode++)
	{
		SubstitutionStructPtr pInsert = new SubstitutionStruct;
		pNodeValues[nName++] >>= pInsert->sFont;
		pNodeValues[nName++] >>= pInsert->sReplaceBy;
		pInsert->bReplaceAlways = *(sal_Bool*)pNodeValues[nName++].getValue();
		pInsert->bReplaceOnScreenOnly = *(sal_Bool*)pNodeValues[nName++].getValue();
		pImpl->aSubstArr.Insert(pInsert, pImpl->aSubstArr.Count());
	}
}
/* -----------------------------18.01.01 12:06--------------------------------

 ---------------------------------------------------------------------------*/
SvtFontSubstConfig::~SvtFontSubstConfig()
{
	delete pImpl;
}
/*-- 18.01.01 12:08:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void SvtFontSubstConfig::Notify( const com::sun::star::uno::Sequence< rtl::OUString >& )
{
}

void SvtFontSubstConfig::Commit()
{
	Sequence<OUString> aNames(1);
	aNames.getArray()[0] = C2U(cReplacement);
	Sequence<Any> aValues(1);
	aValues.getArray()[0].setValue(&bIsEnabled, ::getBooleanCppuType());
	PutProperties(aNames, aValues);

	OUString sNode(C2U(cFontPairs));
	if(!pImpl->aSubstArr.Count())
		ClearNodeSet(sNode);
	else
	{
		Sequence<PropertyValue> aSetValues(4 * pImpl->aSubstArr.Count());
		PropertyValue* pSetValues = aSetValues.getArray();
		sal_Int32 nSetValue = 0;

		const OUString sReplaceFont(C2U(cReplaceFont));
		const OUString sSubstituteFont(C2U(cSubstituteFont));
		const OUString sAlways(C2U(cAlways));
		const OUString sOnScreenOnly(C2U(cOnScreenOnly));

		const uno::Type& rBoolType = ::getBooleanCppuType();
		for(sal_uInt16 i = 0; i < pImpl->aSubstArr.Count(); i++)
		{
			OUString sPrefix(sNode);
			sPrefix += C2U("/_");
			sPrefix += OUString::valueOf((sal_Int32)i);
			sPrefix += C2U("/");

			SubstitutionStructPtr pSubst = pImpl->aSubstArr[i];
			pSetValues[nSetValue].Name = sPrefix; pSetValues[nSetValue].Name += sReplaceFont;
			pSetValues[nSetValue++].Value <<= pSubst->sFont;
			pSetValues[nSetValue].Name = sPrefix; pSetValues[nSetValue].Name += sSubstituteFont;
			pSetValues[nSetValue++].Value <<= pSubst->sReplaceBy;
			pSetValues[nSetValue].Name = sPrefix; pSetValues[nSetValue].Name += sAlways;
			pSetValues[nSetValue++].Value.setValue(&pSubst->bReplaceAlways, rBoolType);
			pSetValues[nSetValue].Name = sPrefix; pSetValues[nSetValue].Name += sOnScreenOnly;
			pSetValues[nSetValue++].Value.setValue(&pSubst->bReplaceOnScreenOnly, rBoolType);
		}
		ReplaceSetProperties(sNode, aSetValues);
	}
}
/*-- 18.01.01 12:08:00---------------------------------------------------

  -----------------------------------------------------------------------*/
sal_Int32 SvtFontSubstConfig::SubstitutionCount() const
{
	return pImpl->aSubstArr.Count();
}
/*-- 18.01.01 12:08:00---------------------------------------------------

  -----------------------------------------------------------------------*/
void SvtFontSubstConfig::ClearSubstitutions()
{
	pImpl->aSubstArr.DeleteAndDestroy(0, pImpl->aSubstArr.Count());
}
/*-- 18.01.01 12:08:00---------------------------------------------------

  -----------------------------------------------------------------------*/
const SubstitutionStruct* SvtFontSubstConfig::GetSubstitution(sal_Int32 nPos)
{
	DBG_ASSERT(nPos >= 0 && nPos < pImpl->aSubstArr.Count(), "illegal array index");
	if(nPos >= 0 && nPos < pImpl->aSubstArr.Count())
		return pImpl->aSubstArr[(sal_uInt16)nPos];
	return 0;
}
/*-- 18.01.01 12:08:01---------------------------------------------------

  -----------------------------------------------------------------------*/
void SvtFontSubstConfig::AddSubstitution(const SubstitutionStruct& rToAdd)
{
	SubstitutionStructPtr pInsert = new SubstitutionStruct(rToAdd);
	pImpl->aSubstArr.Insert(pInsert, pImpl->aSubstArr.Count());
}

void SvtFontSubstConfig::Apply()
{
	OutputDevice::BeginFontSubstitution();

	// Alte Substitution entfernen
	sal_uInt16 nOldCount = OutputDevice::GetFontSubstituteCount();

	while (nOldCount)
		OutputDevice::RemoveFontSubstitute(--nOldCount);

	// Neue Substitution einlesen
    sal_Int32 nCount = IsEnabled() ? SubstitutionCount() : 0;

	for (sal_Int32  i = 0; i < nCount; i++)
	{
	    sal_uInt16 nFlags = 0;
		const SubstitutionStruct* pSubs = GetSubstitution(i);
		if(pSubs->bReplaceAlways)
			nFlags |= FONT_SUBSTITUTE_ALWAYS;
		if(pSubs->bReplaceOnScreenOnly)
			nFlags |= FONT_SUBSTITUTE_SCREENONLY;
		OutputDevice::AddFontSubstitute( String(pSubs->sFont), String(pSubs->sReplaceBy), nFlags );
    }

	OutputDevice::EndFontSubstitution();
}
