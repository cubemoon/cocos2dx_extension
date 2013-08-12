#include "GuidingLayer.h"
#include "EasyUseApi.h"

USING_NS_CC;
USING_NS_CC_EXT;

using namespace bailin;

#define TAG_ID_TIPS_NODE 100

// Ĭ������
bool bailin::GuidingLayer::m_bEnable = true;

static bailin::GuidingLayer* g_sGuidingLayer = NULL;

bool bailin::GuidingLayer::onInitDialog()
{
	return true;
}

bool bailin::GuidingLayer::init()
{
	do
	{
		CC_BREAK_IF(!DialogLayer::init());

		// Ĭ�ϸ�����Ȧ�ƶ�ʱ��Ϊ0.5��
		m_fMotionTime = 0.5f;

		m_nPos = 0;

		// Ĭ��ӵ�н���
		m_bFocused = true;

		// ����ָ��ص���ʼ
		m_iterCurrent = m_listGuidingStep.begin();

		// ʹ��������ɫ͸��
		setOpacity(0);

		// ����һ���µ�CCLayerColor
		m_pMaskLayer = CCLayerColor::create(ccc4(0, 0, 0, 128));
		m_pMaskLayer->setAnchorPoint(CCPointZero);
		m_pMaskLayer->setContentSize(this->getContentSize());

		// ������һ��Node����Ϊ���ü��Ķ����
		m_pClipper = CCClippingNode::create();
		m_pStencilLayer = CCNode::create();

		// ��ӱ��ü���
		m_pClipper->addChild(m_pMaskLayer);
		// ��Ӳü�ģ���
		m_pClipper->setStencil(m_pStencilLayer);

		// �ο�͸�����ɼ������Ԫ��
		m_pClipper->setInverted(true);
		m_pClipper->setAlphaThreshold(0.05f);
		m_pClipper->setContentSize(m_pMaskLayer->getContentSize());

		// �趨λ��
		m_pClipper->setAnchorPoint(CCPointZero);
		m_pClipper->setPosition(CCPointZero);
		addChild(m_pClipper);

		// ������Ȧ
		m_pHole = CCSprite::create("ccbResources/common/light.png");
		m_pHole->setScale(10);
		m_pHole->setPosition(ccpMult(ccpFromSize(this->getContentSize()), 0.5f));
		m_pStencilLayer->addChild(m_pHole);

		return true;

	} while(0);

	return false;
}

void bailin::GuidingLayer::onEnter()
{
	DialogLayer::onEnter();
}

void bailin::GuidingLayer::onExit()
{
	DialogLayer::onExit();

//	CC_SAFE_RELEASE_NULL(g_sGuidingLayer);
}

void bailin::GuidingLayer::AppendGuidingStep( const GUIDING_STEP& gd_step )
{
	m_listGuidingStep.push_back(gd_step);

	m_iterCurrent = m_listGuidingStep.begin();

	int nPos = m_nPos;
	// ���¶�λ
	while (nPos)
	{
		m_iterCurrent++;
		nPos--;
	}
}

void bailin::GuidingLayer::InsertGuidingStep( const GUIDING_STEP& gd_step )
{

}

void bailin::GuidingLayer::NextStep()
{
	// ��������ã���ֱ�ӷ���
	if (!m_bEnable)
	{
		return;
	}

	// ���»�ȡ����
	ResetFocus();

	// ���û�ж�����ֱ���Ƴ�����
	if (m_iterCurrent == m_listGuidingStep.end())
	{
		CCActionInterval* pAction = CCSequence::create(
			CCSpawn::create(CCFadeOut::create(m_fMotionTime), CCScaleTo::create(m_fMotionTime, 3), NULL),
			CCCallFunc::create(this, callfunc_selector(GuidingLayer::ActionCallbackRemoveSelf)),
			NULL
			);
//		CC_SAFE_RELEASE_NULL(g_sGuidingLayer);
		m_pHole->stopAllActions();
		m_pHole->runAction(pAction);

		return;
	}

	if (m_iterCurrent != m_listGuidingStep.end())
	{
		// �Ƴ���ǰ�ü�ģ��������
//		m_pStencilLayer->removeAllChildrenWithCleanup(true);

		if (getChildByTag(TAG_ID_TIPS_NODE))
		{
			removeChildByTag(TAG_ID_TIPS_NODE);
		}

		// �Ƴ�����
		CCLOG(typeid(m_iterCurrent->pTouchedCtl).name());

		if (m_iterCurrent != m_listGuidingStep.begin())
		{
			m_iterCurrent--;
			int nType = m_iterCurrent->nCtrlType;
			switch(nType)
			{
			case TOUCH_CTRL_TYPE_MENU:
				popMenu(dynamic_cast<CCMenu*>(m_iterCurrent->pTouchedCtl));
				break;
			case TOUCH_CTRL_TYPE_EDITBOX:
				popEditBox(dynamic_cast<CCEditBox*>(m_iterCurrent->pTouchedCtl));
				break;
			case TOUCH_CTRL_TYPE_SCROLLVIEW:
				popScrollView(dynamic_cast<CCScrollView*>(m_iterCurrent->pTouchedCtl));
				break;
			case TOUCH_CTRL_TYPE_OTHERCTL:
				popOtherTouchedCtl(dynamic_cast<CCLayer*>(m_iterCurrent->pTouchedCtl));
				break;
			}
			m_iterCurrent++;
		}

		// ��Ӽ���
		switch(m_iterCurrent->nCtrlType)
		{
		case TOUCH_CTRL_TYPE_MENU:
			pushMenu(dynamic_cast<CCMenu*>(m_iterCurrent->pTouchedCtl));
			break;
		case TOUCH_CTRL_TYPE_EDITBOX:
			pushEditBox(dynamic_cast<CCEditBox*>(m_iterCurrent->pTouchedCtl));
			break;
		case TOUCH_CTRL_TYPE_SCROLLVIEW:
			pushScrollView(dynamic_cast<CCScrollView*>(m_iterCurrent->pTouchedCtl));
			break;
		case TOUCH_CTRL_TYPE_OTHERCTL:
			pushOtherTouchedCtl(dynamic_cast<CCLayer*>(m_iterCurrent->pTouchedCtl));
			break;
		}

		// �����ʾ����
		if (m_iterCurrent->pTipsNode)
		{
			m_iterCurrent->pTipsNode->setTag(TAG_ID_TIPS_NODE);
			addChild(m_iterCurrent->pTipsNode);

			// �������ʾ����������
			if (m_iterCurrent->pTipsAction)
			{
				m_iterCurrent->pTipsNode->runAction(m_iterCurrent->pTipsAction);
			}
		}

		// ����һ�ݶ��󣬲��Ҹ������С��λ�ã���ӵ���ǰ��ģ���
		// ���û�и�����Χ�Ľڵ㣬��ֱ���ô����ڵ�����С�ıȶ�
		CCNode* pNode = m_iterCurrent->pRectCtl ? m_iterCurrent->pRectCtl : m_iterCurrent->pTouchedCtl;
		CCAssert(pNode, "GuidingLayer m_iterCurrent->pRectCtl and m_iterCurrent->pTouchedCtl must not be null both");
		if(!pNode)
		{
			return;
		}
		// �õ��Խ��߳���
		CCSize size = pNode->getContentSize();
		float fDiameter = sqrtf(pow(size.width, 2) + pow(size.height, 2));

		CCPoint pos = pNode->getPosition();
		pos = EasyUseApi::getNodeCenterPoint(pNode);

		// ����ϵת��
		pos = pNode->getParent()->convertToWorldSpace(pos);
		pos = m_pStencilLayer->convertToNodeSpace(pos);

		// �߹��ƶ�Ч��
		CCActionInterval* pAction = CCSpawn::create(
			CCMoveTo::create(m_fMotionTime, pos),
			CCScaleTo::create(m_fMotionTime, fDiameter / m_pHole->getContentSize().width),
			NULL
			);

		m_pHole->stopAllActions();
		m_pHole->runAction(pAction);

		m_nPos++;
		++m_iterCurrent;
	}
}

void bailin::GuidingLayer::PreviousStep()
{

}

void bailin::GuidingLayer::InitCurrIterator()
{
	m_iterCurrent = m_listGuidingStep.begin();
}

GuidingLayer* bailin::GuidingLayer::sharedGuidingLayer()
{
	if (!g_sGuidingLayer)
	{
		g_sGuidingLayer = GuidingLayer::create();
	}

	return g_sGuidingLayer;
}

void bailin::GuidingLayer::EnableGuide( bool bEnable )
{
	m_bEnable = bEnable;
}

bool bailin::GuidingLayer::ccTouchBegan( cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent )
{
	DialogLayer::ccTouchBegan(pTouch, pEvent);

	if (m_iterCurrent != m_listGuidingStep.end() && m_iterCurrent->bClickOutside)
	{
		// �������Ӧ����Ҳִ����һ������
		NextStep();
	}

	return true;
}

bailin::GuidingLayer::~GuidingLayer()
{
//	CC_SAFE_RELEASE_NULL(m_pHole);
}

void bailin::GuidingLayer::SetMaskOpacity(GLubyte opacity)
{
	m_pMaskLayer->setOpacity(opacity);
}

void bailin::GuidingLayer::SetMaskColor3B( cocos2d::ccColor3B color )
{
	m_pMaskLayer->setColor(color);
}

void bailin::GuidingLayer::SetHightLightMoveTime( float fTime )
{
	m_fMotionTime = fTime;
}

void bailin::GuidingLayer::ActionCallbackRemoveSelf()
{
	removeFromParent();
}

void bailin::GuidingLayer::ReleaseFocus()
{
	if (!m_bFocused)
	{
		return;
	}

	// ɾ�� ��ʾ����
	removeChildByTag(TAG_ID_TIPS_NODE);

	// �ͷŴ���
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);

	// �õ���Ļ�ĶԽ���
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	float fDiameter = sqrtf(pow(winSize.width, 2) + pow(winSize.height, 2));

	CCPoint pos = ccpMult(ccpFromSize(winSize), 0.5f);
	// �������
	CCActionInterval* pAction = CCSpawn::create(
		CCMoveTo::create(m_fMotionTime, pos),	// �����ƶ�
		CCScaleTo::create(m_fMotionTime, fDiameter/m_pHole->getContentSize().width),
		NULL
		);
	m_pHole->stopAllActions();
	m_pHole->runAction(pAction);

	// ʧȥ����
	m_bFocused = false;
}

void bailin::GuidingLayer::ResetFocus()
{
	if (!m_bFocused)
	{
		CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority - 1, true);
		m_bFocused = true;
	}
}

void bailin::GuidingLayer::JumpNextStep()
{
	if (m_iterCurrent == m_listGuidingStep.end())
	{
		// ����
		NextStep();
		return;
	}

	m_iterCurrent++;
}
