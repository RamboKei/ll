#pragma once


#include "..\WorldDefine\role_att.h"
#include "..\WorldDefine\remote_role.h"
#include "..\WorldDefine\msg_item.h"
#include "..\WorldDefine\msg_combat.h"
#include "..\WorldDefine\msg_role_title.h"
#include "PetEvent.h"
#include "..\WorldDefine\action.h"

class Role;
class Player;
class LocalPlayer;
class NetSession;
class ShadowPool;
class KeyCodeFrame;
struct tagNS_SetTransportMap;
struct tagLPRoleStateChangeEvent;
struct tagMousePointRoleIDChangeEvent;
struct tagRolePickEvent;
struct tagLeaveCombatEvent;
struct tagNS_AppearanceTransform;
struct tagNS_SyncAvataHairAndFace;
struct tagNS_RoleBeAttacked;
struct tagEnterCombatEvent;
struct tagNPCTalkEvent;
struct tagNS_WillShutdownWorld;
struct tagNS_IsDressUpAgree;
struct tagNS_ChangeClergy;
struct tagNS_RemoteRoleSkin;
struct tagNS_Skill;
struct tagNS_SyncFamilyActive;
struct tagNS_PlsInputReactiveKeyCode;
struct tagNS_ReactiveKeyCode;
struct tagNS_RemoteHolySoulAttChange;
struct tagNS_CallShengLing;
struct tagNS_CallShengLingDisappear;
class VirtualRole;
enum EPLayActionType;
enum ESoulAttOptType;
/** \class RoleMgr
	\brief 角色管理器
*/
class RoleMgr
{
public:
	enum{MAX_VIEW_DIST=8};
public:
	RoleMgr(void);
	~RoleMgr(void);

	/** 进入游戏世界时初始化
	*/
	void Init(DWORD localPlayerID);
	/** 离开游戏世界时释放所有数据
	*/
	void Destroy();
	/** 进入游戏地图
	*/
	void OnOpenMap(SceneGraph* pSG, const Vector3& localPlayerPos, const Vector3& localPlayerFaceto);
	/** 关闭地图时释放除本地玩家以外的所有角色数据
	*/
	void OnCloseMap(SceneGraph* pSG);


	DWORD OnHideOtherPlayers( tagGameEvent *pEvent )
	{
		m_bHideOtherPlayers = true; 
		return 0; 
	}
	DWORD OnShowOtherPlayers( tagGameEvent *pEvent )
	{ 
		m_bHideOtherPlayers = false;
		return 0;
	}

	map<DWORD, tagClientRemoteRoleFabaoInfo>& GetTrumpEffectMap() { return m_mapTrumpEffect; }

	map<DWORD, DWORD>& GetFashionEffectMap() { return m_mapFashionEffect; }

	/** 设置视野距离级别
	*/
	void SetViewDistLevel( const int viewDistLevel );
	int  GetViewDistLevel() const { return m_viewDistLevel; }
	/** 根据角色ID查找角色
		\param bAddReqIfNotFind:是否向服务器请求角色数据
	*/
	Role* FindRole(DWORD roleID,bool bAddReqIfNotFind=false,bool bFindDeadCreatureList=false);
	/** 获得本地玩家角色
	*/
	LocalPlayer* GetLocalPlayer(){ return m_pLocalPlayer; }
	/** 获取角色容器
	*/
	const map<DWORD,Role*>& GetRoleMap() { return m_roleMap; }
	/** 获取本地玩家ID
	*/
	DWORD GetLocalPlayerID();
	/** 每帧更新
	*/
	void Update(SceneGraph* pSG);
	
	/** 角色拾取
	    \bIgnorePlayer 是否忽略玩家
	*/
	DWORD PickRole(const Ray& mouseRay, const BOOL bIgnorePlayer);

	// 自动拾取角色
	bool AutoPickRole(DWORD dwRoleID);
	/** 显示/隐藏阴影
	*/
	void EnalbeShadow( const BOOL bEnalbe ) { m_bEnalbeShadow = bEnalbe; }	
	/** 角色是否在角色视野内
	*/
	bool IsInViewDist( Role* pRole );

	/** 本地玩家是否远离NPC/玩家
	*/
	bool IsOutValidDist(DWORD dwRoleID);
	bool IsOutFaceDist(DWORD dwRoleID);
	// 队友是否在有效范围内
	bool IsInTeamBufDis( DWORD );
	/** 判断AABBox内是否含有角色
	*/
	bool IsAABBoxCollideRole(const AABBox &box, bool bOnlyNPC = true);
	/** 更新可交互地物任务相关特效
	*/
	void UpdataGameObjectQuestEffect();
	static RoleMgr* Inst();
	// 得到活动怪物位置
	const vector<tagACreaturePosInfo> &GetMonsterInfo() { return m_vecActiveMons; }
	// 是否在同一师门
	bool IsInSameMasterGuild( Player *pPlayer );
	VOID PopUpVoteUI();
	VOID DestroyVoteUI();
	BOOL IsEnemy( DWORD dwRoleID );
	bool IsTransforming(Player* pPlayer);

	//DWORD OnNS_Skill(tagNS_Skill* pMsg, DWORD dwParam);

	/** 通过角色id获取其虚拟角色(元神觉醒)*/
	VirtualRole* GetVirtualRole(DWORD dwRoleID);
	/** 通过角色id获取是否有虚拟角色(元神觉醒)*/
	BOOL HasVirtualRole(DWORD dwRoleID);
	/** 同步虚拟角色和人物作战动作(元神觉醒)*/
	VOID SynVRCombatAction( DWORD dwRoleID, tstring strActionName );
	//元神控制
	VOID HolyManCtrl(DWORD dwRoleID, EPLayActionType eType, DWORD dwBuffID=GT_INVALID);
	//重载元神模型, 返回是否重载   
	BOOL ReloadHolyManModel(DWORD dwRoleID, DWORD dwHolyID, ESoulAttOptType eOpt );

private:
	// 活动怪物
	DWORD OnNS_ActiveMoster( tagNS_SyncActiveCreaturesPos *pNetCmd, DWORD );

	/** 添加到角色请求列表
	*/
	void AddReqRole(DWORD dwRoleID);
	/** 发送角色请求
	*/
	void SendReqRoleList();
	/** 释放一个角色，不允许释放玩家自已
	*/
	void FreeRole(DWORD roleID,SceneGraph* pSG);
	//---------------------------------------------------------------------
	//  网络消息处理                                                              
	//---------------------------------------------------------------------
	/** 同步本地玩家家族活跃度
	*/
	DWORD OnNS_SyncFamilyActive(tagNS_SyncFamilyActive* pNetCmd, DWORD);
	/** 获取远程玩家变身皮肤
	*/
	DWORD NetGetRemoteRoleTransSkin(tagNS_RemoteRoleSkin* pNetCmd, DWORD);
	/** 删除远程玩家
	*/
	DWORD NetRemoveRemote(tagNS_RemoveRemote* pNetCmd, DWORD);
	/** 初始化本地玩家属性
	*/
	DWORD NetInitLocalPlayerAtt(tagNS_GetRoleInitState_Att* pNetCmd, DWORD);
	/** 获取远程玩家信息
	*/
	DWORD NetGetRemoteRoleState(tagNS_GetRemoteRoleState* pNetCmd, DWORD);
	/** 获取周围生物信息
	*/
	DWORD NetGetRemoteCreatureState(tagNS_GetRemoteCreatureState* pNetCmd, DWORD);
    /** 获取周围宠物信息
    */
    DWORD NetGetRemotePetState(tagNS_GetRemotePetState* pNetCmd, DWORD);
    /** 获取周围坐骑信息
    */
    DWORD OnGetPetMount(tagEnterMount* pGameEvent);
    /** 删除坐骑
    */
    DWORD OnLeavePetMount(tagLeaveMount* pGameEvent);
	/** 本地玩家单项属性变化
	*/
	DWORD NetRoleAttChangeSingle(tagNS_RoleAttChangeSingle* pNetCmd, DWORD);
	/** 本地玩家多项属性变化 
	*/
	DWORD NetRoleAttChangeMutiple(tagNS_RoleAttChangeMutiple* pNetCmd, DWORD);
	/** 远程角色单项属性变化
	*/
	DWORD NetRemoteAttChangeSingle(tagNS_RemoteAttChangeSingle* pNetCmd, DWORD);
	/** 远程角色多项属性变化
	*/
	DWORD NetRemoteAttChangeMutiple(tagNS_RemoteAttChangeMutiple* pNetCmd, DWORD);
	/** 远程角色帮派信息变化
	*/
	DWORD NetRemoteGuildInfoChange(tagNS_RemoteRoleGuildInfoChange *pNetCmd, DWORD);
	/** 玩家Avatar变化
	*/
	DWORD NetAvatarEquipChange(tagNS_AvatarEquipChange* pNetCmd, DWORD);
    //! 玩家称号变化
    DWORD NetRoleTitleChange(tagNS_RoleTitleChangeBroadcast* pNetCmd, DWORD);
	/** 玩家Avatar全部更新（时装/装备切换）
	*/
	DWORD NetAvatarEquip(tagNS_AvatarEquip* pNetCmd, DWORD);
	/** 龙魂效果改变
	*/
	DWORD NetEquipEffectChange(tagNS_EquipEffectChange* pNetCmd, DWORD);
	/** 套装特效改变
	*/
	DWORD NetSuitEffectChange(tagNS_SuitEffect* pNetCmd, DWORD);
	/** 增加角色Buff
	*/
	DWORD NetAddRoleBuff(tagNS_AddRoleBuff* pNetCmd, DWORD);
	/** 移除角色Buff
	*/
	DWORD NetRemoveRoleBuff(tagNS_RemoveRoleBuff* pNetCmd, DWORD);
	/** 更新角色Buff
	*/
	DWORD NetUpdateRoleBuff(tagNS_UpdateRoleBuff* pNetCmd, DWORD);
	/** 更新本地玩家属性投点
	*/
	DWORD NetRoleAttPointAddChange(tagNS_RoleAttPointAddChange* pNetCmd, DWORD);
	/** 投点返回
	*/
	DWORD NetRoleBidAttPoint(tagNS_RoleBidAttPoint* pNetCmd, DWORD);
	/** 洗点返回
	*/
	DWORD NetRoleClearAttPoint(tagNS_RoleClearAttPoint* pNetCmd, DWORD);
	/** 添加角色自身状态
	*/
	DWORD NetSetRoleStateEx(tagNS_SetRoleStateEx* pNetCmd, DWORD);
	/** 取消角色自身状态
	*/
	DWORD NetUnsetRoleStateEx(tagNS_UnSetRoleStateEx* pNetCmd, DWORD);
	/** 设置玩家的显示设置
	*/
	DWORD NetRoleSetDisplay(tagNS_RoleSetDisplay* pNetCmd, DWORD);
	/** 设置本地玩家的个人设置
	*/
	DWORD NetSetPersonalSet(tagNS_SetPersonalSet* pNetCmd, DWORD);
	/** 怪物所属改变
	*/
	DWORD NetCreatureTaggedOwnerChange(tagNS_CreatureTaggedOwnerChange* pNetCmd, DWORD);
	/** 角色阴影更新
	*/
    /** 角色可记录传送符改变
    */
    DWORD OnNS_SetTransportMap(tagNS_SetTransportMap* pNetCmd, DWORD);

	//玩家升级提示，主要是为新手做指引
	DWORD OnNS_RoleLevelChange( tagNS_RoleLevelChange* pNetCmd, DWORD );

	// 玩家名字改变
	DWORD OnNetRename( tagNS_LPRename *pNetCmd, DWORD );
	// 玩家名字改变,通知周围玩家
	DWORD NetRoleRenameBroadcast( tagNS_RoleRename *pNetCmd, DWORD );
	// 玩家神职改变，通知周围玩家
	DWORD OnNS_ChangeClergy(tagNS_ChangeClergy* pNetCmd, DWORD);
	// 玩家元神发生变化
	DWORD OnNS_RemoteHolySoulAttChange( tagNS_RemoteHolySoulAttChange *pNetCmd, DWORD );


	DWORD OnNS_PlsInputReactiveKeyCode(tagNS_PlsInputReactiveKeyCode* pNetCmd, DWORD);
	DWORD OnNS_ReactiveKeyCode(tagNS_ReactiveKeyCode* pNetCmd, DWORD);
    void ShowSetTransportErrorMsg(DWORD dwErrCode);

	VOID UpdataRoleShadow();
	/** 角色是否需要隐藏
	*/
	bool GetRoleHide( Role* pRole );
	/** 更新高亮显示
	*/
	void UpdataHightLight( Role* pRole );
	/** 进出安全区提示
	*/
	DWORD OnLpChangeStateEvent( tagLPRoleStateChangeEvent *pEvent );
	// 响应鼠标指向改变事件
	DWORD OnMousePointRoleIDChangeEvent(tagMousePointRoleIDChangeEvent *pEvent);
	// 响应角色选中事件
	DWORD OnRolePickEvent(tagRolePickEvent *pEvent);
	// 小龙女投票
	DWORD OnRoleVote( tagMsgInputBoxEvent *pEvent );
	DWORD OnNetVote( tagNS_VoteForDrangonGirl *pNetCmd, DWORD );
	DWORD OnNetChangeAvatar( tagNS_AppearanceTransform *pNetCmd, DWORD );
	DWORD OnEvent( tagGameEvent* pEvent );
	// 法宝特效同步
	DWORD OnNetRRoleChangeFabao( tagNS_RemoteRoleChangeFabao *pNetCmd, DWORD );
	// 玩家被攻击
	DWORD OnNetBeAttacked( tagNS_RoleBeAttacked *pNetCmd, DWORD );
	// 脱离战斗状态
	DWORD OnRoleLeaveCombatEvent( tagLeaveCombatEvent *pEvent );
	// 进入战斗状态
	DWORD OnRoleEnterCombatEvent( tagEnterCombatEvent *pEvent );
	// NPC喊话
	DWORD OnNPCTalkEvent( tagNPCTalkEvent *pEvent );
	// 关服倒计时
	DWORD OnNetShutDownServer( tagNS_WillShutdownWorld *pNetCmd, DWORD );
	// 吃经验丹
	DWORD OnNetUseExpPilular( tagNS_UseExpPilular *pNetCmd, DWORD );
	// 好友装扮自己
	DWORD OnNetFriendBeautyMe( tagNS_IsDressUpAgree *pNetCmd, DWORD );
	// 装扮自己
	DWORD OnEventFriendBeautyMe( tagMsgBoxEvent *pGameEvent );
	// 召唤圣灵（包括本地玩家和远程玩家）
	DWORD OnNS_CallShengLing(tagNS_CallShengLing* pCmd, DWORD);
	// 圣灵消失（包括本地玩家和远程玩家）
	DWORD OnNS_CallShengLingDisappear(tagNS_CallShengLingDisappear*pCmd, DWORD);
	// 圣灵消失（包括本地玩家和远程玩家）
	//DWORD OnNS_UnSetRoleState(tagNS_UnSetRoleState* pCmd, DWORD);

	//同步玩家或者怪物阵营信息
	DWORD OnNS_SyncCamp(tagNS_SyncCamp* pCmd, DWORD);
	/**家在元神的VirtualRole模型*/
	BOOL LoadHolyManModel(Player* pPlayer, Vector3 v3FaceTo, DWORD dwModelID/*, tagAvatarAtt	Avatar, tagAvatarEquip	AvatarEquip, tagDisplaySet	DisplaySet*/ );
public:
	//圣魔圣灵VirtualRole模型加载（上面那个会被替代）
	BOOL LoadHolyManModel(BOOL bIsHide = TRUE);
	//加载远程玩家的虚拟模型
	BOOL LoadHolyManModel(DWORD dwRoleID, DWORD dwHolyTypeID, DWORD dwEatFairyNum);
	// 元神属性变化对人物属性的影响
	//DWORD OnNS_GetRoleAttHolySoulEffect( tagNS_GetRoleAttHolySoulEffect *pNetCmd, DWORD );
	//发送云联暂停游戏http请求
	void SendyunLianPauthGame();
private:
	TSFPTrunk<RoleMgr>			m_trunk;
	TObjRef<Util>				m_pUtil;
	TObjRef<NetCmdMgr>			m_pCmdMgr;
	TObjRef<NetSession>			m_pSession;
    TObjRef<GameFrameMgr>       m_pFrameMgr;
	ShadowPool*                 m_pShadowPool;
	BOOL                        m_bEnalbeShadow;

	LocalPlayer*				m_pLocalPlayer;		 // 本地玩家
	map<DWORD,Role*>			m_roleMap;			 // 角色map
	list<Role*>					m_listDeadCreature;	 // 死亡生物列表
	pair<DWORD, list<DWORD> >	m_paReqRole;		 // 角色请求列表

	bool                        m_bHideOtherPlayers; // 是否隐藏其他玩家
	int							m_viewDistLevel;     // 角色视野级别
	float                       m_fViewDist;         // 角色视野距离
	DWORD						m_dwRolePointEffectID;// 角色选中特效ID
	vector<tagACreaturePosInfo>		m_vecActiveMons;		// 活动怪物位置
	
	map<DWORD, tagClientRemoteRoleFabaoInfo>	m_mapTrumpEffect;		// 附近玩家法宝特效
	map<DWORD, DWORD> m_mapFashionEffect;			// 附近玩家时装特效
	set<DWORD>				m_setEnemy;				// 攻击自己的仇人列表
	DWORD					m_dwRecivMsgTime;		// 收到消息的时间			
	KeyCodeFrame*			m_pKeyCodeFrame; //key码

	//VirtualRole*				m_pVirtualRole;
	map<DWORD, VirtualRole*> m_mapVirtualRole;
	//Vector3						m_v3FaceTo; //本地玩家朝向

};
