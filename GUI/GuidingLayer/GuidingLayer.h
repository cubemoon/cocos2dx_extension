/**
* @author Jason Tou
* @date 20130723
* @comment �����ǻ���DialogLayer�������㣬����������������
* 1. ʹ��create������
* 2. ����GUIDING_STEP�����ڵ�
*  2.1 ���ö�Ӧ�Ĵ�����Ӧ�ؼ��� pTouchedCtl 
*  2.2 ���ö�Ӧ��ENUM_CTRL_TYPE���͡� nCtrlType
*  2.4 ������������������Ĺ�Ȧ��С��������Ǹ����ؼ����ζԽ��ߴ�СΪ��Ȧֱ���� pRectCtl
*      ����˿ؼ�δ�ṩ�����Դ�����Ӧ�ؼ��Ĵ�СΪ��׼��
*  2.5 �������������ڵ��Ӧ�Ķ�����ʾ�ؼ�����Ҫ���ⲿretain��pTipsNode
*  2.6 ���ö���ؼ��Ķ�������Ҫ���ⲿretain�� pTipsAction
*      ����ֻ������pTipsNodeִ��runAction��pTipsNode���ڲ���������ͨ����������
*  2.7 ���õ�ǰ�����Ƿ���Ե����Ļ������������� bClickOutside�� Ĭ��Ϊfalse
* 3. ʹ��AppendGuidingStep�����������ڵ�������ӵ������С�����������˳�����������������
*    ���Ź��̣�Ŀǰ�ݲ�֧�ֻ��ˣ�PreviouseAction�ǿպ���
* 4. �ظ�2��3ֱ�������Ϻ󣬵���InitCurrIterator������ʹ����ָ��ص���һ����λ��
* 5. ��ӵ�ǰ�㵽������Ҫ���������Ĳ���
* 6. ����NextAction()ʹ����������һ��һ�����£�����������Ƴ��ò�
*/

#ifndef __GUIDING_LAYER_h__
#define __GUIDING_LAYER_h__

#include "DialogLayer.h"

enum ENUM_CTRL_TYPE
{
	TOUCH_CTRL_TYPE_NULL = 0,
	TOUCH_CTRL_TYPE_MENU,			// CCMenu
	TOUCH_CTRL_TYPE_EDITBOX,		// CCEditBox
	TOUCH_CTRL_TYPE_SCROLLVIEW,		// CCScrollView
	TOUCH_CTRL_TYPE_OTHERCTL		// Other
};
/*
* @brief ���������ṹ��
*
*/
typedef struct tagGuidingStep
{
	/* ������Ӧ�����Ŀؼ� */
	cocos2d::CCNode* pTouchedCtl;
	/* �ؼ����� */
	int nCtrlType;
	/* ���ƴ�С�Ŀؼ� */
	cocos2d::CCNode* pRectCtl;
	/* ��Ӧ��ʾ����Ļ�ϵ���ʾ����������ȫ������ */
	cocos2d::CCNode* pTipsNode;
	/* ��ʾ���ֵĶ��� */
	cocos2d::CCActionInterval* pTipsAction;
	/* �����ִ����һ�������� */
	cocos2d::CCRect clickRect;
	/* �Ƿ���������ִ����һ�� */
	bool bClickOutside;

	tagGuidingStep()
	{
		Clear();
	}

	void Clear()
	{
		pTouchedCtl = NULL;
		pRectCtl = NULL;
		pTipsNode = NULL;
		pTipsAction = NULL;
		clickRect.setRect(0, 0, 0, 0);
		nCtrlType = TOUCH_CTRL_TYPE_NULL;
		bClickOutside = false;
	}

} GUIDING_STEP;

class GuidingLayer : public DialogLayer
{
public:
	virtual ~GuidingLayer();

	CREATE_FUNC(GuidingLayer);

	virtual bool onInitDialog();

	virtual bool init();

	virtual void onEnter();

	virtual void onExit();

public:
	/**
	* @brief ����һ��ȫ�־�̬�����������������
	*
	*/
	static GuidingLayer* sharedGuidingLayer();

	/**
	* @brief �ڶ���ĩβ�����������
	*
	*/
	void AppendGuidingStep(const GUIDING_STEP& gd_step);

	/**
	* @brief ��һ������
	*
	*/
	void NextStep();

	/**
	* @brief ����һ������
	*/
	void JumpNextStep();

	/**
	* @brief ��һ������
	*
	*/
	void PreviousStep();

	/**
	* @brief ��ʼ��ǰ��������λ��
	*
	*/
	void InitCurrIterator();

	/**
	* @brief �Ƿ�����������������
	* @param �Ƿ�����
	*
	*/
	static void EnableGuide(bool bEnable);

	/**
	* @brief �������ֲ��͸����
	*/
	void SetMaskOpacity(GLubyte opacity);

	/**
	* @brief �������ֲ���ɫ
	*/
	void SetMaskColor3B(cocos2d::ccColor3B color);

	/**
	* @brief ���ø�����Ȧ�ƶ���ʱ��
	*/
	void SetHightLightMoveTime(float fTime);

	/**
	* @brief �����������ͷ���Ļ��׽�������Լ�ʹ�ø�����ʧ
	* ����������������������״̬ʱ�����Ե���ִ�е��øýӿ�
	* ��ʱֹͣ����Ĵ�����׽�͸���Ч��
	*/
	void ReleaseFocus();

	/**
	* @brief ������Ӽ���
	*/
	void ResetFocus();

	void ActionCallbackRemoveSelf();
private:
	/**
	* @brief �ڵ�ǰλ�������������
	*
	*/
	void InsertGuidingStep(const GUIDING_STEP& gd_step);

	virtual bool ccTouchBegan( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent );


protected:
	/* ��ǰλ�� */
	std::list<GUIDING_STEP>::iterator m_iterCurrent;

	int m_nPos;

	/* ���������ݶ��У����飩 */
	std::list<GUIDING_STEP> m_listGuidingStep;

	/* �ü����� */
	cocos2d::CCClippingNode* m_pClipper;

	/*
	���òü������ģ���
	���еĿɴ����ؼ����������
	*/
	cocos2d::CCNode* m_pStencilLayer;

	/* ��ɫ���� */
	cocos2d::CCLayerColor* m_pMaskLayer;

	/* ����ԲȦ */
	cocos2d::CCSprite* m_pHole;

	/* �Ƿ����� */
	static bool m_bEnable;

	/* ������Ȧ�ƶ���ʱ�� �����ڵ���NextActionǰ��ָ̬��*/
	float m_fMotionTime;

	/* �Ƿ�ʧȥ���� */
	bool m_bFocused;
};

#endif // __GUIDING_LAYER_h__