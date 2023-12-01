#import <Foundation/Foundation.h>

// 云请求
typedef enum _PY_CLOUD_STATE
{
    CLOUD_STATE_REQUEST = 0,
    CLOUD_STATE_DIFF,
    CLOUD_STATE_SAME,
    CLOUD_STATE_FAIL,
    CLOUD_STATE_MAX,
    CLOUD_STATE_NONE = CLOUD_STATE_MAX,
} PY_CLOUD_STATE;

// 输入状态
typedef enum _PY_INPUT_STATE
{
    INPUT_STATE_NORMAL = 0,
    INPUT_STATE_MODE_V_ENTER,
    INPUT_STATE_MODE_V_IN,
    INPUT_STATE_MODE_U_ENTER,
    INPUT_STATE_MODE_U_IN,
} PY_INPUT_STATE;

#define PY_CARET_MOVE_HOME          1
#define PY_CARET_MOVE_END           2
#define PY_CARET_MOVE_LEFT_CHAR     3
#define PY_CARET_MOVE_RIGHT_CHAR    4
#define PY_CARET_MOVE_LEFT_UNIT     5
#define PY_CARET_MOVE_RIGHT_UINT    6

#define PY_FILTER_NONE              0
#define PY_FILTER_SINGLE_WORD       1

#define PY_INPUT_STATE_MODEV        1
#define PY_INPUT_STATE_MODEU        2

@interface PalmEngine : NSObject
{
    char *           _pyEngineInst;
    void *           _ftHandle;
    char *           _pUserDictPtr;
    int              _pUserDictSize;

    NSData*           _pySysLex;
    
    int                 _commitTimes;
    int                 _candMaxNum;
    int                 _candHotItem;
    int                 _pageCurrent;
    
    NSMutableArray     *_candList;
    NSMutableArray     *_pageList;
}

-(void) reset;
-(void) resetContext;

-(void) save:(BOOL) forceWriteFile;

-(BOOL) setOptions:(const int *) options;
-(void) getOptions:(int *) options;

-(BOOL) processKey:(unichar) keyId;

-(NSString *) getKeyString;
-(NSString *) getCompString:(int *) caretPos withSplit:(BOOL) bSplit;

- (BOOL) isCaretAtCompEnd;

- (BOOL) isFirstItemHasFocus;

-(BOOL) moveCaret:(int) operId;

-(BOOL) setCandFilter:(int) filterId;
-(int) getCandFilter;

-(int) getCandTotal;
-(NSString *) getCandItem:(int) candItemId;
-(NSString *) selCandItem:(int) candItemId;
-(BOOL) delCandItem:(int) candItemId;

-(void) resetPage;
-(void) setPageItemMaxNum:(int) pageItemMaxNum;

-(BOOL) testPageUp;
-(BOOL) testPageDown;
-(BOOL) pageUp;
-(BOOL) pageDown;

-(NSArray *) getPageItems;
-(int) getPageItemTotal;
-(NSString *) getPageItem:(int) pageItemId;
-(NSString *) selPageItem:(int) pageItemId;
-(BOOL) delPageItem:(int) pageItemId;

-(BOOL) moveHotspotLeft;
-(BOOL) moveHotspotRight;
-(int) getHotspotPageItemId;

-(BOOL) isFreeEdit;

-(void) setTradMode:(BOOL) bTrad;
-(BOOL) isTradMode;

-(void) undoLastCommit;
-(NSString *) getCandPrefix;

-(PY_CLOUD_STATE) getCloudState;

-(int) getInputState;

-(BOOL) updateCateDicts:(NSArray*)arrayCateName SPAssistDict:(NSString*)strSPAssistGUID;

@end
