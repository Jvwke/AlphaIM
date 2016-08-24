#ifndef _IMPREEDIT_H_
#define _IMPREEDIT_H_

#include <string>
#include <deque>
#include "MutexLock.h"

#include "aim.h"
#include "iIM.h"
#include "MessageQueue.h"
#include "CharUtil.h"

/*
 * Situation:
 *     vv: a valid input.
 *     hansx: chinese character.
 *     hans1: has be selected by pressing digital key.
 *     cc   : is going to be slelected by user pressing digital key. 
 *     dd   : Another valid py.
 *
 *  input sequences         vv - bb    - vv - cc    - dd 
 *                          vv - hans1 - vv - hans2 - hans3
 *
 * =================================================================
 *  ci            vvhans1
 *  input         vvccdd
 *  candidate     vvhans2(0)hans3(0)
 *
 *  items         hans2
 *  ci_items      hans1
 *
 * ================================================================
 * IM UI:
 * input        ci + input
 * candidate    ci + candidate.
 * items        items
 */

using namespace std;


#define CACHE_ITEMS_MAX  10

#define ICWIN_W  480
#define ICWIN_H  64

typedef struct {
    int x,y,w,h;
} ICRect;

typedef struct {
    int	keysym;
    int	modifier;
    int	modifier_mask;
} TriggerKey;

enum PREEDIT_KEY {
    NONE_KEY,
    FORWARD_KEY,
    TRIGGER_ON_KEY,
    TRIGGER_OFF_KEY,
    SWITCH_CE_KEY,
    SWITCH_CEP_KEY,
    CONVERT_KEY,
    COMMIT_KEY,
    PAGEUP_KEY,
    PAGEDOWN_KEY,
    CANCEL_KEY,
};

class IMPreeditCallback {
public:
    IMPreeditCallback(void* priv=NULL): opaque(priv) {};
    virtual void onIMOn(void* priv) {};
    virtual void onIMOff(void* priv) {};
    virtual void onCommit(void* priv, string candiate) = 0;
    virtual ICRect onGetRect(void* priv) = 0;
    void *opaque;
};

class IMPreedit {
public:
    IMPreedit();
    virtual ~IMPreedit() {};
    virtual int handleKey(int keycode, int modifier, char *key, IMPreeditCallback *callback) {return NONE_KEY;};
    void guiReload(IMPreeditCallback *callback);
    void clear();
    void close(bool bReset=false);

    iIM  *im;
    bool bRefreshWin;

protected:
    void add(char *key);
    void del();
    void page(int pg);
    bool commit(int i);

    void doSwitchCE(IMPreeditCallback *callback);
    void doSwitchCEPun();
    void doPageup();
    void doPagedown();
    bool doInput(char *key);
    void doClose();
    void doCommit(int i, IMPreeditCallback *callback);

    void guiAction(int id);
    void guiShowCandidate(IMPreeditCallback *callback);

    bool isMatchKeys(int keycode, int modifier, TriggerKey *trigger);
    u32  mapCNPun(char *key);
    string  mapCNPunToU8Str(char *key);

    bool m_bStart;
    string m_input;
    string m_ci;
    string m_candidate;
    int m_curPage; // Start from 1.

    bool   m_bTrigger;
    bool   m_bCN;
    bool   m_bCNPun;  //Chinese punctuation
    bool   m_bPreQuo;

    MutexCriticalSection m_cs;

    deque<IMItem> m_items;
    deque<IMItem> m_ciItems;
    deque<IMItem> m_uiItems;
};

#endif
