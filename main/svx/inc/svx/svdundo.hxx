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



#ifndef _SVDUNDO_HXX
#define _SVDUNDO_HXX

#ifndef _SOLAR_HRC
#include <svl/solar.hrc>
#endif
#include <svl/undo.hxx>
#include <tools/contnr.hxx>
#include <tools/gen.hxx>
#include <svx/svdtypes.hxx> // fuer enum RepeatFuncts
#include <svx/svdsob.hxx>
#include "svx/svxdllapi.h"

//************************************************************
//   Vorausdeklarationen
//************************************************************

class SfxItemSet;
class SfxStyleSheet;
class SdrView;
class SdrPageView;
class SdrModel;
class SdrObject;
class SdrPage;
class SdrObjList;
class SdrLayer;
class SdrLayerAdmin;
class SdrObjGeoData;
class OutlinerParaObject;

//************************************************************
//   SdrUndoAction
//
// Abstrakte Basisklasse fuer alle UndoActions der DrawingEngine
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoAction : public SfxUndoAction
{
protected:
	SdrModel&					rMod;

protected:
	SdrUndoAction(SdrModel& rNewMod)
	:	rMod(rNewMod)
	{}

public:
	TYPEINFO();
	virtual sal_Bool CanRepeat(SfxRepeatTarget& rView) const;
	virtual void Repeat(SfxRepeatTarget& rView);

	virtual String GetRepeatComment(SfxRepeatTarget& rView) const;
	virtual String GetSdrRepeatComment(SdrView& rView) const;

	virtual bool CanSdrRepeat(SdrView& rView) const;
	virtual void SdrRepeat(SdrView& rView);
};

//************************************************************
//   SdrUndoGroup
//
// Zusammenfassung beliebig vieler einzelner UndoActions zu einer einzigen
// UndoAction. Beim Undo() werden die Einzelactions rueckwaets aufgerufen
// (die zuletzt angehaengte zuerst...), beim Redo() dagegen entspricht die
// Aufrufreihenfolge der Einfuegereihenfolge.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoGroup : public SdrUndoAction
{
protected:
	Container					aBuf;

	// Beschreibung der Action, nicht expandiert (beinhaltet %O)
	String						aComment;
	String						aObjDescription;

	SdrRepeatFunc				eFunction;

public:
	SdrUndoGroup(SdrModel& rNewMod);
	SdrUndoGroup(SdrModel& rNewMod,const String& rStr);
	virtual ~SdrUndoGroup();

	void Clear();
	sal_uIntPtr GetActionCount() const { return aBuf.Count(); }
	SdrUndoAction* GetAction(sal_uIntPtr nNum) const { return (SdrUndoAction*)(aBuf.GetObject(nNum)); }
	void AddAction(SdrUndoAction* pAct);
	void push_front( SdrUndoAction* pAct );

	void SetComment(const String& rStr) { aComment=rStr; }
	void SetObjDescription(const String& rStr) { aObjDescription=rStr; }
	virtual String GetComment() const;
	virtual String GetSdrRepeatComment(SdrView& rView) const;

	virtual void Undo();
	virtual void Redo();

	virtual bool CanSdrRepeat(SdrView& rView) const;
	virtual void SdrRepeat(SdrView& rView);
	void SetRepeatFunction(SdrRepeatFunc eFunc) { eFunction=eFunc; }
	SdrRepeatFunc GetRepeatFunction() const { return eFunction; }
};

//************************************************************
//   SdrUndoObj
//
// Abstrakte Basisklasse fuer alle UndoActions die mit Objekten zu tun haben.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoObj : public SdrUndoAction
{
protected:
	SdrObject*					pObj;

protected:
	SdrUndoObj(SdrObject& rNewObj);

	void ImpTakeDescriptionStr(sal_uInt16 nStrCacheID, String& rStr, FASTBOOL bRepeat=sal_False) const;

	static void GetDescriptionStringForObject( const SdrObject& _rForObject, sal_uInt16 nStrCacheID, String& rStr, FASTBOOL bRepeat = sal_False );

    // #94278# new method for evtl. PageChange at UNDO/REDO
	void ImpShowPageOfThisObject();
};

//************************************************************
//   SdrUndoAttrObj
//
// Aenderung der Objektattribute.
// Action direkt vor dem Setzen der neuen Attribute konstruieren.
// Auch fuer StyleSheets
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoAttrObj : public SdrUndoObj
{
protected:
	SfxItemSet*					pUndoSet;
	SfxItemSet*					pRedoSet;
	SfxItemSet*					pRepeatSet;

	// oder besser den StyleSheetNamen merken?
	SfxStyleSheet*				pUndoStyleSheet;
	SfxStyleSheet*				pRedoStyleSheet;
	SfxStyleSheet*				pRepeatStyleSheet;
	FASTBOOL					bStyleSheet;
	FASTBOOL					bHaveToTakeRedoSet;

	// Bei Zuweisung von TextItems auf ein Zeichenobjekt mit Text:
	OutlinerParaObject*			pTextUndo;
	// #i8508#
	// The text rescue mechanism needs also to be implemented for redo actions.
	OutlinerParaObject*			pTextRedo;

	// Wenn sich um ein Gruppenobjekt handelt:
	SdrUndoGroup*				pUndoGroup;

public:
	SdrUndoAttrObj(SdrObject& rNewObj, FASTBOOL bStyleSheet1=sal_False, FASTBOOL bSaveText=sal_False);
	virtual ~SdrUndoAttrObj();
	void SetRepeatAttr(const SfxItemSet& rSet);
	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
	virtual String GetSdrRepeatComment(SdrView& rView) const;

	virtual void SdrRepeat(SdrView& rView);
	virtual bool CanSdrRepeat(SdrView& rView) const;
};

//************************************************************
//   SdrUndoMoveObj
//
// Blosses verschieben eines Objektes.
// Action direkt vor dem Verschieben konstruieren.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoMoveObj : public SdrUndoObj
{
protected:
	Size						aDistance;     // Entfernung, um die verschoben wird

public:
	SdrUndoMoveObj(SdrObject& rNewObj): SdrUndoObj(rNewObj) {}
	SdrUndoMoveObj(SdrObject& rNewObj, const Size& rDist): SdrUndoObj(rNewObj),aDistance(rDist) {}

	void SetDistance(const Size& rDist) { aDistance=rDist; }
	const Size& GetDistance() const { return aDistance; }

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
	virtual String GetSdrRepeatComment(SdrView& rView) const;

	virtual void SdrRepeat(SdrView& rView);
	virtual bool CanSdrRepeat(SdrView& rView) const;
};

//************************************************************
//   SdrUndoGeoObj
//
// Aenderung der Geometrie eines Objektes.
// Action direkt vor der geometrischen Transformation konstruieren.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoGeoObj : public SdrUndoObj
{
protected:
	SdrObjGeoData*				pUndoGeo;
	SdrObjGeoData*				pRedoGeo;
	// Wenn sich um ein Gruppenobjekt handelt:
	SdrUndoGroup*				pUndoGroup;

public:
	SdrUndoGeoObj(SdrObject& rNewObj);
	virtual ~SdrUndoGeoObj();

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
};

//************************************************************
//   SdrUndoObjList
//
// Manipulationen an einer ObjList: Neues Obj, DeleteObj, SetObjZLevel, Grouping, ...
// Abstrakte Basisklasse.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoObjList : public SdrUndoObj {
	bool    					bOwner;

protected:
	SdrObjList*					pObjList;
	SdrView*					pView;      // um bei ObjDel, Undo die
	SdrPageView*				pPageView;  // Selektion widerherstellen zu koennen
	sal_uInt32						nOrdNum;
	// Bei einem Undo/Redo findet moeglicherweise Uebereignung des Objektes
	// statt. Im Dtor wird das Obj deleted, wenn bOwner==TRUE

protected:
	SdrUndoObjList(SdrObject& rNewObj, bool bOrdNumDirect = false);
	virtual ~SdrUndoObjList();

	void SetView(SdrView* pView1, SdrPageView* pPageView1) { pView=pView1; pPageView=pPageView1; }
	bool IsOwner() { return bOwner; }
	void SetOwner(bool bNew);
};

//************************************************************
//   SdrUndoRemoveObj
//
// Entfernen Objekts aus einer Objektliste. Innerhalb einer UndoGroup
// mit korrospondierenden Inserts zu verwenden.
// Action vor dem entfernen aus der ObjList konstruieren.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoRemoveObj : public SdrUndoObjList
{
public:
	SdrUndoRemoveObj(SdrObject& rNewObj, FASTBOOL bOrdNumDirect=sal_False)
	: SdrUndoObjList(rNewObj,bOrdNumDirect) {}

	virtual void Undo();
	virtual void Redo();
};

//************************************************************
//   SdrUndoInsertObj
//
// Einfuegen Objekts in eine Objektliste. Innerhalb einer UndoGroup
// mit korrospondierenden Removes zu verwenden.
// Action vor dem entfernen aus der ObjList konstruieren.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoInsertObj : public SdrUndoObjList
{
public:
	SdrUndoInsertObj(SdrObject& rNewObj, FASTBOOL bOrdNumDirect=sal_False)
	:	SdrUndoObjList(rNewObj,bOrdNumDirect) {}

	virtual void Undo();
	virtual void Redo();
};

//************************************************************
//   SdrUndoDelObj
//
// Loeschen eines Objekts.
// Action vor dem entfernen aus der ObjList konstruieren.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoDelObj : public SdrUndoRemoveObj
{
public:
	SdrUndoDelObj(SdrObject& rNewObj, FASTBOOL bOrdNumDirect=sal_False)
	:	SdrUndoRemoveObj(rNewObj,bOrdNumDirect) { SetOwner(sal_True); }

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
	virtual String GetSdrRepeatComment(SdrView& rView) const;

	virtual void SdrRepeat(SdrView& rView);
	virtual bool CanSdrRepeat(SdrView& rView) const;
};

//************************************************************
//   SdrUndoNewObj
//
// Einfuegen eines neuen Objektes.
// Action nach dem einfuegen in die ObjList konstruieren.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoNewObj : public SdrUndoInsertObj
{
public:
	SdrUndoNewObj(SdrObject& rNewObj, FASTBOOL bOrdNumDirect=sal_False)
	:	SdrUndoInsertObj(rNewObj,bOrdNumDirect) {}

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;

    static  String GetComment( const SdrObject& _rForObject );
};

//************************************************************
//   SdrUndoReplaceObj
//
// Austausch eines Objektes.
// Action vor dem Replace an der ObjList konstruieren.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoReplaceObj : public SdrUndoObj
{
	bool    					bOldOwner;
	bool    					bNewOwner;

protected:
	SdrObjList*					pObjList;
	sal_uInt32						nOrdNum;
	SdrObject*					pNewObj;

public:
	SdrUndoReplaceObj(SdrObject& rOldObj1, SdrObject& rNewObj1, bool bOrdNumDirect = false);
	virtual ~SdrUndoReplaceObj();

	virtual void Undo();
	virtual void Redo();

	bool IsNewOwner() { return bNewOwner; }
	void SetNewOwner(bool bNew);

	bool IsOldOwner() { return bOldOwner; }
	void SetOldOwner(bool bNew);
};

//************************************************************
//   SdrUndoCopyObj
//
// Kopieren eines Objekts
// Action nach dem einfuegen in die ObjList konstruieren.
//
//************************************************************

class SdrUndoCopyObj : public SdrUndoNewObj
{
public:
	SdrUndoCopyObj(SdrObject& rNewObj, FASTBOOL bOrdNumDirect=sal_False)
	:	SdrUndoNewObj(rNewObj,bOrdNumDirect) {}

	virtual String GetComment() const;
};

//************************************************************
//   SdrUndoObjOrdNum
//************************************************************

class SdrUndoObjOrdNum : public SdrUndoObj
{
protected:
	sal_uInt32						nOldOrdNum;
	sal_uInt32						nNewOrdNum;

public:
	SdrUndoObjOrdNum(SdrObject& rNewObj, sal_uInt32 nOldOrdNum1, sal_uInt32 nNewOrdNum1);

	virtual void Undo();
	virtual void Redo();

	virtual String   GetComment() const;
};

//////////////////////////////////////////////////////////////////////////////
// #i11702#

class SVX_DLLPUBLIC SdrUndoObjectLayerChange : public SdrUndoObj
{
protected:
	SdrLayerID					maOldLayer;
	SdrLayerID					maNewLayer;

public:
	SdrUndoObjectLayerChange(SdrObject& rObj, SdrLayerID aOldLayer, SdrLayerID aNewLayer);

	virtual void Undo();
	virtual void Redo();
};

//************************************************************
//   SdrUndoObjSetText
//************************************************************

class SVX_DLLPUBLIC SdrUndoObjSetText : public SdrUndoObj
{
protected:
	OutlinerParaObject*			pOldText;
	OutlinerParaObject*			pNewText;
	FASTBOOL					bNewTextAvailable;
	sal_Bool						bEmptyPresObj;
	sal_Int32					mnText;

public:
	SdrUndoObjSetText(SdrObject& rNewObj, sal_Int32 nText );
	virtual ~SdrUndoObjSetText();

	FASTBOOL IsDifferent() const { return pOldText!=pNewText; }
	void AfterSetText();

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
	virtual String GetSdrRepeatComment(SdrView& rView) const;

	virtual void SdrRepeat(SdrView& rView);
	virtual bool CanSdrRepeat(SdrView& rView) const;
};

// --> OD 2009-07-09 #i73249#
class SdrUndoObjStrAttr : public SdrUndoObj
{
public:
    enum ObjStrAttrType
    {
        OBJ_NAME,
        OBJ_TITLE,
        OBJ_DESCRIPTION
    };

protected:
    const ObjStrAttrType meObjStrAttr;
    const String msOldStr;
    const String msNewStr;

public:
    SdrUndoObjStrAttr( SdrObject& rNewObj,
                       const ObjStrAttrType eObjStrAttr,
                       const String& sOldStr,
                       const String& sNewStr);

    virtual void Undo();
    virtual void Redo();

    virtual String GetComment() const;
};
// <--

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@     @@@@  @@  @@ @@@@@ @@@@@
//  @@    @@  @@ @@  @@ @@    @@  @@
//  @@    @@  @@ @@  @@ @@    @@  @@
//  @@    @@@@@@  @@@@  @@@@  @@@@@
//  @@    @@  @@   @@   @@    @@  @@
//  @@    @@  @@   @@   @@    @@  @@
//  @@@@@ @@  @@   @@   @@@@@ @@  @@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************
//   SdrUndoLayer
//
// Abstrakte Basisklasse fuer alle UndoActions die mit SdrLayer zu tun haben.
//
//************************************************************

class SdrUndoLayer : public SdrUndoAction
{
protected:
	SdrLayer*					pLayer;
	SdrLayerAdmin*				pLayerAdmin;
	sal_uInt16						nNum;
	FASTBOOL					bItsMine;

protected:
	SdrUndoLayer(sal_uInt16 nLayerNum, SdrLayerAdmin& rNewLayerAdmin, SdrModel& rNewModel);
	virtual ~SdrUndoLayer();
};

//************************************************************
//   SdrUndoNewLayer
//
// Einfuegen eines neuen Layer. Action nach dem Einfuegen konstruieren.
//
//************************************************************

class SdrUndoNewLayer : public SdrUndoLayer
{
public:
	SdrUndoNewLayer(sal_uInt16 nLayerNum, SdrLayerAdmin& rNewLayerAdmin, SdrModel& rNewModel)
	: SdrUndoLayer(nLayerNum,rNewLayerAdmin,rNewModel) {}

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
};

//************************************************************
//   SdrUndoDelLayer
//
// Loeschen eines Layer. Action vor dem Remove konstruieren
//
//************************************************************

class SdrUndoDelLayer : public SdrUndoLayer
{
public:
	SdrUndoDelLayer(sal_uInt16 nLayerNum, SdrLayerAdmin& rNewLayerAdmin, SdrModel& rNewModel)
	: SdrUndoLayer(nLayerNum,rNewLayerAdmin,rNewModel) { bItsMine=sal_True; }

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
};

//************************************************************
//   SdrUndoMoveLayer
//
// Verschieben eines Layer. Action vor dem Verschieben konstruieren.
//
//************************************************************

class SdrUndoMoveLayer : public SdrUndoLayer
{
	sal_uInt16						nNeuPos;

public:
	SdrUndoMoveLayer(sal_uInt16 nLayerNum, SdrLayerAdmin& rNewLayerAdmin, SdrModel& rNewModel, sal_uInt16 nNeuPos1)
	:	SdrUndoLayer(nLayerNum,rNewLayerAdmin,rNewModel), nNeuPos(nNeuPos1) {}

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@@@@   @@@@   @@@@  @@@@@  @@@@
//  @@  @@ @@  @@ @@  @@ @@    @@  @@
//  @@  @@ @@  @@ @@     @@    @@
//  @@@@@  @@@@@@ @@ @@@ @@@@   @@@@
//  @@     @@  @@ @@  @@ @@        @@
//  @@     @@  @@ @@  @@ @@    @@  @@
//  @@     @@  @@  @@@@@ @@@@@  @@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************
//   SdrUndoPage
//
// Abstrakte Basisklasse fuer alle UndoActions die mit SdrPages zu tun haben.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoPage : public SdrUndoAction
{
protected:
	SdrPage&					mrPage;

protected:
	void ImpInsertPage(sal_uInt16 nNum);
	void ImpRemovePage(sal_uInt16 nNum);
	void ImpMovePage(sal_uInt16 nOldNum, sal_uInt16 nNewNum);

protected:
	SdrUndoPage(SdrPage& rNewPg);

	void ImpTakeDescriptionStr(sal_uInt16 nStrCacheID, String& rStr, sal_uInt16 n=0, FASTBOOL bRepeat=sal_False) const;
};

//************************************************************
//   SdrUndoPageList
//
// Manipulationen an einer PageList: Neue Page, DeletePage, MovePage(ChangePageNum)
// Abstrakte Basisklasse.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoPageList : public SdrUndoPage
{
protected:
	sal_uInt16						nPageNum;

	// Bei einem Undo/Redo findet moeglicherweise Uebereignung der Page
	// statt. Im Dtor wird die Page deleted, wenn bItsMine==TRUE
	FASTBOOL					bItsMine;

protected:
	SdrUndoPageList(SdrPage& rNewPg);
	virtual ~SdrUndoPageList();
};

//************************************************************
//   SdrUndoDelPage
//
// Loeschen einer Page.
// Action vor dem entfernen aus der List konstruieren.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoDelPage : public SdrUndoPageList
{
	// Beim loeschen einer MasterPage merke ich mir in dieser UndoGroup
	// alle Beziehungen der Zeichenseiten zu der geloeschten MasterPage
	SdrUndoGroup*				pUndoGroup;

public:
	SdrUndoDelPage(SdrPage& rNewPg);
	virtual ~SdrUndoDelPage();

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
	virtual String GetSdrRepeatComment(SdrView& rView) const;

	virtual void SdrRepeat(SdrView& rView);
	virtual bool CanSdrRepeat(SdrView& rView) const;
};

//************************************************************
//   SdrUndoNewPage
//
// Einfuegen einer neuen Page.
// Action nach dem einfuegen in die Liste konstruieren.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoNewPage : public SdrUndoPageList
{
public:
	SdrUndoNewPage(SdrPage& rNewPg): SdrUndoPageList(rNewPg) {}

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
};

//************************************************************
//   SdrUndoCopyPage
//
// Kopieren einer Page
// Action nach dem einfuegen in die Liste konstruieren.
//
//************************************************************

class SdrUndoCopyPage : public SdrUndoNewPage
{
public:
	SdrUndoCopyPage(SdrPage& rNewPg): SdrUndoNewPage(rNewPg) {}

	virtual String GetComment() const;
	virtual String GetSdrRepeatComment(SdrView& rView) const;

	virtual void SdrRepeat(SdrView& rView);
	virtual bool CanSdrRepeat(SdrView& rView) const;
};

//************************************************************
//   SdrUndoSetPageNum
//
// Verschieben der Page innerhalb der Liste
// Action vor dem Verschieben der Page konstruieren.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoSetPageNum : public SdrUndoPage
{
protected:
	sal_uInt16						nOldPageNum;
	sal_uInt16						nNewPageNum;

public:
	SdrUndoSetPageNum(SdrPage& rNewPg, sal_uInt16 nOldPageNum1, sal_uInt16 nNewPageNum1)
	:	SdrUndoPage(rNewPg),nOldPageNum(nOldPageNum1),nNewPageNum(nNewPageNum1) {}

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @@   @@  @@@@   @@@@  @@@@@@ @@@@@ @@@@@   @@@@@   @@@@   @@@@  @@@@@  @@@@
//  @@@ @@@ @@  @@ @@  @@   @@   @@    @@  @@  @@  @@ @@  @@ @@  @@ @@    @@  @@
//  @@@@@@@ @@  @@ @@       @@   @@    @@  @@  @@  @@ @@  @@ @@     @@    @@
//  @@@@@@@ @@@@@@  @@@@    @@   @@@@  @@@@@   @@@@@  @@@@@@ @@ @@@ @@@@   @@@@
//  @@ @ @@ @@  @@     @@   @@   @@    @@  @@  @@     @@  @@ @@  @@ @@        @@
//  @@   @@ @@  @@ @@  @@   @@   @@    @@  @@  @@     @@  @@ @@  @@ @@    @@  @@
//  @@   @@ @@  @@  @@@@    @@   @@@@@ @@  @@  @@     @@  @@  @@@@@ @@@@@  @@@@
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************
//   SdrUndoPageMasterPage
//
// Abstrakte Basisklasse fuer alle UndoActions die mit
// MasterPage-Beziehungen zu tun haben.
//
//************************************************************

class SdrUndoPageMasterPage : public SdrUndoPage
{
protected:
	sal_Bool					mbOldHadMasterPage;
	SetOfByte					maOldSet;
	sal_uInt16					maOldMasterPageNumber;

protected:
	SdrUndoPageMasterPage(SdrPage& rChangedPage);

public:
	SVX_DLLPUBLIC virtual ~SdrUndoPageMasterPage();
};

//************************************************************
//   SdrUndoPageRemoveMasterPage
//
// Entfernen einer MasterPage von einer Zeichenseite.
// Action vor dem Entfernen des MasterPageDescriptors erzeugen.
//
//************************************************************

class SdrUndoPageRemoveMasterPage : public SdrUndoPageMasterPage
{
public:
	SdrUndoPageRemoveMasterPage(SdrPage& rChangedPage);

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
};

//************************************************************
//   SdrUndoPageChangeMasterPage
//
// Aenderung des MasterPageDescriptors (z.B. Aendern der VisibleLayer).
// Action vor der Aenderung am MasterPageDescriptor erzeugen.
//
//************************************************************

class SVX_DLLPUBLIC SdrUndoPageChangeMasterPage : public SdrUndoPageMasterPage
{
protected:
	sal_Bool					mbNewHadMasterPage;
	SetOfByte					maNewSet;
	sal_uInt16					maNewMasterPageNumber;

public:
	SdrUndoPageChangeMasterPage(SdrPage& rChangedPage);

	virtual void Undo();
	virtual void Redo();

	virtual String GetComment() const;
};

///////////////////////////////////////////////////////////////////////

/** the SdrUndoFactory can be set and retrieved from the SdrModel.
	It is used by the drawing layer implementations to create undo actions.
	It can be used by applications to create application specific undo actions.
*/
class SVX_DLLPUBLIC SdrUndoFactory
{
public:
	// shapes
    virtual ~SdrUndoFactory();
	virtual SdrUndoAction* CreateUndoMoveObject( SdrObject& rObject );
	virtual SdrUndoAction* CreateUndoMoveObject( SdrObject& rObject, const Size& rDist );
	virtual SdrUndoAction* CreateUndoGeoObject( SdrObject& rObject );
	virtual SdrUndoAction* CreateUndoAttrObject( SdrObject& rObject, bool bStyleSheet1 = false, bool bSaveText = false );
	virtual SdrUndoAction* CreateUndoRemoveObject( SdrObject& rObject, bool bOrdNumDirect = false);
	virtual SdrUndoAction* CreateUndoInsertObject( SdrObject& rObject, bool bOrdNumDirect = false);
	virtual SdrUndoAction* CreateUndoDeleteObject( SdrObject& rObject, bool bOrdNumDirect = false);
	virtual SdrUndoAction* CreateUndoNewObject( SdrObject& rObject, bool bOrdNumDirect = false);
	virtual SdrUndoAction* CreateUndoCopyObject( SdrObject& rObject, bool bOrdNumDirect = false);

	virtual SdrUndoAction* CreateUndoObjectOrdNum( SdrObject& rObject, sal_uInt32 nOldOrdNum1, sal_uInt32 nNewOrdNum1);

	virtual SdrUndoAction* CreateUndoReplaceObject( SdrObject& rOldObject, SdrObject& rNewObject, bool bOrdNumDirect = false );
	virtual SdrUndoAction* CreateUndoObjectLayerChange( SdrObject& rObject, SdrLayerID aOldLayer, SdrLayerID aNewLayer );
	virtual SdrUndoAction* CreateUndoObjectSetText( SdrObject& rNewObj, sal_Int32 nText );

    // --> OD 2009-07-09 #i73249#
    virtual SdrUndoAction* CreateUndoObjectStrAttr( SdrObject& rObject,
                                                    SdrUndoObjStrAttr::ObjStrAttrType eObjStrAttrType,
                                                    String sOldStr,
                                                    String sNewStr );
    // <--

	// layer
	virtual SdrUndoAction* CreateUndoNewLayer(sal_uInt16 nLayerNum, SdrLayerAdmin& rNewLayerAdmin, SdrModel& rNewModel);
	virtual SdrUndoAction* CreateUndoDeleteLayer(sal_uInt16 nLayerNum, SdrLayerAdmin& rNewLayerAdmin, SdrModel& rNewModel);
	virtual SdrUndoAction* CreateUndoMoveLayer(sal_uInt16 nLayerNum, SdrLayerAdmin& rNewLayerAdmin, SdrModel& rNewModel, sal_uInt16 nNeuPos1);

	// page
	virtual SdrUndoAction*	CreateUndoDeletePage(SdrPage& rPage);
	virtual SdrUndoAction* CreateUndoNewPage(SdrPage& rPage);
	virtual SdrUndoAction* CreateUndoCopyPage(SdrPage& rPage);
	virtual SdrUndoAction* CreateUndoSetPageNum(SdrPage& rNewPg, sal_uInt16 nOldPageNum1, sal_uInt16 nNewPageNum1);

	// master page
	virtual SdrUndoAction* CreateUndoPageRemoveMasterPage(SdrPage& rChangedPage);
	virtual SdrUndoAction* CreateUndoPageChangeMasterPage(SdrPage& rChangedPage);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //_SVDUNDO_HXX

